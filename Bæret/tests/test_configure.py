import importlib.util
from pathlib import Path
import tempfile
import unittest
from unittest.mock import Mock

spec = importlib.util.spec_from_file_location('pi_configure', Path(__file__).resolve().parents[1] / 'configure.py')
configure = importlib.util.module_from_spec(spec)
spec.loader.exec_module(configure)


class PiConfigurationTest(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.root = Path(self.temp.name)
        self.config = self.root / 'scoreboard.env'

    def tearDown(self):
        self.temp.cleanup()

    def create(self, password='spaces "quotes" \\ $ % æ'):
        return configure.prepare_environment(self.config, ask=lambda _: '', ask_secret=lambda _: password)

    def test_first_install_roundtrips_password_and_creates_session_key(self):
        values = self.create()
        self.assertEqual(configure.read_environment(self.config), values)
        self.assertEqual(len(values['SCORE_SECRET_KEY']), 64)

    def test_reinstall_preserves_existing_file_and_credentials_without_prompt(self):
        values = self.create()
        self.config.write_text(self.config.read_text(encoding='utf-8') + '# keep comment\nCUSTOM="keep me"\n', encoding='utf-8')
        before = self.config.read_bytes()
        no_prompt = Mock(side_effect=AssertionError('Unexpected prompt'))
        actual = configure.prepare_environment(self.config, ask=no_prompt, ask_secret=no_prompt)
        self.assertEqual(self.config.read_bytes(), before)
        self.assertEqual(actual['MQTT_PASSWORD'], values['MQTT_PASSWORD'])
        self.assertEqual(actual['CUSTOM'], 'keep me')

    def test_invalid_existing_topology_is_rejected_without_writing(self):
        self.create()
        self.config.write_text(self.config.read_text(encoding='utf-8').replace('127.0.0.1', '192.168.1.2'), encoding='utf-8')
        before = self.config.read_bytes()
        with self.assertRaises(ValueError):
            configure.prepare_environment(self.config)
        self.assertEqual(self.config.read_bytes(), before)

    def test_placeholder_migration_keeps_backup_and_admin_password(self):
        original = 'SCORE_PASSWORD=existing-admin\nMQTT_PASSWORD=bytt-til-mqtt-passordet\n'
        self.config.write_text(original, encoding='utf-8')
        values = self.create('real-password')
        self.assertEqual(values['SCORE_PASSWORD'], 'existing-admin')
        self.assertEqual(self.config.with_name('scoreboard.env.before-elektra').read_text(encoding='utf-8'), original)

    def test_newline_in_password_is_rejected_before_writing(self):
        with self.assertRaises(ValueError):
            self.create('password\nMQTT_ENABLED=false')
        self.assertFalse(self.config.exists())

    def test_password_merge_retains_other_users_and_replaces_only_selected_user(self):
        actual = configure.merge_password('other:unchanged\nBIAISbroker:old\n', 'BIAISbroker', 'BIAISbroker:new')
        self.assertEqual(actual, 'other:unchanged\nBIAISbroker:new\n')

    def test_existing_biais_listener_is_reused_without_second_listener(self):
        conf_dir = self.root / 'conf.d'
        conf_dir.mkdir()
        password = self.root / 'passwd'
        legacy = conf_dir / 'biais.conf'
        legacy.write_text(f'allow_anonymous false\npassword_file {password}\nlistener 1883\n', encoding='utf-8')
        selected, create = configure.listener_config(conf_dir, self.root / 'mosquitto.conf', password)
        self.assertEqual(selected, legacy)
        self.assertFalse(create)
        self.assertFalse((conf_dir / 'elektra.conf').exists())

    def test_custom_listener_is_rejected_without_changes(self):
        conf_dir = self.root / 'conf.d'
        conf_dir.mkdir()
        custom = conf_dir / 'custom.conf'
        original = 'listener 1883\nallow_anonymous true\n'
        custom.write_text(original, encoding='utf-8')
        with self.assertRaises(ValueError):
            configure.listener_config(conf_dir, self.root / 'mosquitto.conf', self.root / 'passwd')
        self.assertEqual(custom.read_text(encoding='utf-8'), original)

    def test_clean_broker_selects_new_managed_config(self):
        conf_dir = self.root / 'conf.d'
        selected, create = configure.listener_config(conf_dir, self.root / 'mosquitto.conf', self.root / 'passwd')
        self.assertEqual(selected, conf_dir / 'elektra.conf')
        self.assertTrue(create)


if __name__ == '__main__':
    unittest.main()
