import os
# Hide the pygame hello banner
os.environ["PYGAME_HIDE_SUPPORT_PROMPT"] = "1"

import sys
import socket
import struct
import pygame
import math
import random

from loadmap import get_csv_files, load_map
from ball   import Ball

# ─── Configuration ────────────────────────────────
TILE_SIZE    = 15
UDP_PORT     = 5005
FPS          = 60
RESTITUTION  = 0.7

# Scoring parameters
P0           = 10000
HALF_LIFE    = 30.0
STAR_BONUS   = 1000
HOLE_PENALTY = 500

# decay constant
k = math.log(2) / HALF_LIFE

# UI panel
PANEL_WIDTH = 240
PANEL_BG    = (40, 40, 40)
TEXT_COLOR  = (255, 255, 255)

COLORS = {
    0: (200,200,200),  # empty
    1: ( 50, 50, 50),  # wall
    2: (100,  0,  0),  # hole
    3: (  0,200,  0),  # start
    4: (  0,  0,200),  # finish
    5: (255,215,  0),  # star
    6: (255,  0,255),  # checkpoint
}

# ─── Paths ────────────────────────────────────────
BASE_DIR   = os.path.dirname(os.path.abspath(__file__))
ASSETS_DIR = os.path.join(BASE_DIR, "assets")

# ─── MQTT Configuration ───────────────────────────
MQTT_ENABLED      = True
MQTT_BROKER       = "localhost"
MQTT_PORT         = 1883
MQTT_TOPIC_SCORE  = None  # set dynamically

try:
    import paho.mqtt.client as mqtt
except ImportError:
    mqtt = None

# ─── Campaign Configuration ───────────────────────
CAMPAIGN_LEVELS      = 5
CAMPAIGN_MAP_NAMES   = [f"map{i}.csv" for i in range(1, CAMPAIGN_LEVELS+1)]

# ─── Jumpscare Configuration ──────────────────────
SCARE_ENABLED          = True
SCARE_ON_LAST_MAP_ONLY = True
SCARE_PROB             = 0.003   # 0.3% per star on last map
SCARE_DURATION_MS      = 600
SCARE_IMAGE_PATH       = os.path.join(ASSETS_DIR, "jumpscare.png")
SCARE_SOUND_PATH       = os.path.join(ASSETS_DIR, "scream.mp3")  # mp3/ogg/wav

SCARE_IMG              = None
SCARE_SOUND            = None
SCARE_SOUND_IS_MUSIC   = False  # True for mp3 via mixer.music


# ──────────────── Rendering & helpers ────────────────
def render_grid(screen, grid):
    for y,row in enumerate(grid):
        for x,cell in enumerate(row):
            pygame.draw.rect(
                screen,
                COLORS.get(cell,(255,0,255)),
                (x*TILE_SIZE, y*TILE_SIZE, TILE_SIZE, TILE_SIZE)
            )

def find_tile(grid, val):
    for y,row in enumerate(grid):
        for x,cell in enumerate(row):
            if cell==val:
                return (x,y)
    return None

def end_screen(screen, w, h, final_score):
    big = pygame.font.Font(None,48)
    small = pygame.font.Font(None,32)
    l1 = big.render("🎉 Campaign Complete! 🎉", True, TEXT_COLOR)
    l2 = small.render(f"Total Score: {final_score}", True, TEXT_COLOR)
    l3 = small.render("Press Q to Quit", True, (200,200,200))
    r1 = l1.get_rect(center=(w//2, h//2-50))
    r2 = l2.get_rect(center=(w//2, h//2))
    r3 = l3.get_rect(center=(w//2, h//2+50))
    screen.fill((30,30,30))
    screen.blit(l1, r1)
    screen.blit(l2, r2)
    screen.blit(l3, r3)
    pygame.display.flip()
    while True:
        for e in pygame.event.get():
            if e.type==pygame.QUIT: return False
            if e.type==pygame.KEYDOWN and e.key==pygame.K_q: return False
        pygame.time.wait(100)

def start_screen(screen, w, h):
    title  = pygame.font.Font(None, 56).render("TiltMaze Campaign", True, (240,240,240))
    startf = pygame.font.Font(None, 40)
    hint   = pygame.font.Font(None, 28).render("Press ENTER or SPACE to Start (Q to Quit)", True, (200,200,200))

    btn_text = startf.render("Start", True, (20,20,20))
    btn_w, btn_h = 220, 70
    btn_rect = pygame.Rect(0, 0, btn_w, btn_h)
    btn_rect.center = (w//2, h//2 + 40)

    while True:
        screen.fill((18,18,18))
        screen.blit(title, title.get_rect(center=(w//2, h//2 - 80)))
        pygame.draw.rect(screen, (230,230,230), btn_rect, border_radius=12)
        screen.blit(btn_text, btn_text.get_rect(center=btn_rect.center))
        screen.blit(hint, hint.get_rect(center=(w//2, h//2 + 130)))
        pygame.display.flip()

        for e in pygame.event.get():
            if e.type == pygame.QUIT:
                return False
            if e.type == pygame.MOUSEBUTTONDOWN and e.button == 1:
                if btn_rect.collidepoint(e.pos):
                    return True
            if e.type == pygame.KEYDOWN:
                if e.key in (pygame.K_RETURN, pygame.K_SPACE):
                    return True
                if e.key == pygame.K_q:
                    return False
        pygame.time.wait(16)

def text_input_screen(screen, w, h, label="Skriv inn gruppe_id:"):
    font_label = pygame.font.Font(None, 40)
    font_input = pygame.font.Font(None, 36)
    font_hint  = pygame.font.Font(None, 24)

    input_rect = pygame.Rect(0, 0, 420, 48)
    input_rect.center = (w//2, h//2)
    text = ""
    error  = ""

    while True:
        screen.fill((18,18,18))
        surf_label = font_label.render(label, True, (230,230,230))
        screen.blit(surf_label, surf_label.get_rect(center=(w//2, h//2 - 80)))

        pygame.draw.rect(screen, (230,230,230), input_rect, border_radius=8)
        inner = input_rect.inflate(-6, -6)
        pygame.draw.rect(screen, (30,30,30), inner, border_radius=8)

        surf_text = font_input.render(text, True, (230,230,230))
        screen.blit(surf_text, (inner.x+10, inner.y+10))

        hint = "Trykk ENTER for å bekrefte, ESC for å avbryte"
        surf_hint = font_hint.render(hint, True, (180,180,180))
        screen.blit(surf_hint, surf_hint.get_rect(center=(w//2, h//2 + 70)))

        if error:
            surf_err = font_hint.render(error, True, (255, 90, 90))
            screen.blit(surf_err, surf_err.get_rect(center=(w//2, h//2 + 100)))

        pygame.display.flip()

        for e in pygame.event.get():
            if e.type == pygame.QUIT:
                return None
            if e.type == pygame.KEYDOWN:
                if e.key == pygame.K_RETURN:
                    if text.strip():
                        return text.strip()
                    else:
                        error = "gruppe_id kan ikke være tom."
                elif e.key == pygame.K_ESCAPE:
                    return None
                elif e.key == pygame.K_BACKSPACE:
                    text = text[:-1]
                else:
                    if 32 <= e.key <= 126:
                        text += e.unicode
        pygame.time.wait(16)

def intermission_screen(screen, w, h, level_idx, level_score, total_score, next_map, gruppe_id):
    big   = pygame.font.Font(None, 48)
    small = pygame.font.Font(None, 28)

    lines = [
        f"Gruppe: {gruppe_id}",
        f"Level {level_idx} ferdig!",
        f"Score denne runden: {level_score}",
        f"Total score: {total_score}",
        f"Neste map: {next_map}",
        "Trykk SPACE for å fortsette, eller Q for å avslutte"
    ]
    screen.fill((20,20,20))
    for i, txt in enumerate(lines):
        surf = (big if i==1 else small).render(txt, True, (230,230,230))
        rect = surf.get_rect(center=(w//2, h//2 - 110 + i*40))
        screen.blit(surf, rect)
    pygame.display.flip()

    while True:
        for e in pygame.event.get():
            if e.type == pygame.QUIT:
                return False
            if e.type == pygame.KEYDOWN:
                if e.key == pygame.K_SPACE: return True
                if e.key == pygame.K_q:     return False
        pygame.time.wait(50)

def published_screen(screen, w, h, total_score, topic):
    big   = pygame.font.Font(None, 48)
    small = pygame.font.Font(None, 28)
    lines = [
        "✅ Score publisert til server!",
        f"Topic: {topic}",
        f"Score: {total_score}",
        "Trykk en tast for å fortsette"
    ]
    screen.fill((18,18,18))
    for i, txt in enumerate(lines):
        surf = (big if i==0 else small).render(txt, True, (230,230,230))
        rect = surf.get_rect(center=(w//2, h//2 - 60 + i*40))
        screen.blit(surf, rect)
    pygame.display.flip()

    waiting = True
    while waiting:
        for e in pygame.event.get():
            if e.type == pygame.QUIT:
                return
            if e.type == pygame.KEYDOWN:
                waiting = False
        pygame.time.wait(30)

# ─── Jumpscare helpers ────────────────────────────
def load_jumpscare_assets():
    """Load jumpscare image/sound. Uses mixer.music for MP3."""
    global SCARE_IMG, SCARE_SOUND, SCARE_SOUND_IS_MUSIC
    if not SCARE_ENABLED:
        return
    # image
    try:
        if os.path.exists(SCARE_IMAGE_PATH):
            img = pygame.image.load(SCARE_IMAGE_PATH).convert()
            SCARE_IMG = img
        else:
            print(f"⚠️  Jumpscare-bilde ikke funnet: {SCARE_IMAGE_PATH}")
    except Exception as e:
        print(f"⚠️  Kunne ikke laste jumpscare-bilde: {e}")
    # sound
    try:
        ext = os.path.splitext(SCARE_SOUND_PATH)[1].lower()
        if ext == ".mp3":
            SCARE_SOUND_IS_MUSIC = True
            if not pygame.mixer.get_init():
                pygame.mixer.init()
        else:
            if os.path.exists(SCARE_SOUND_PATH):
                if not pygame.mixer.get_init():
                    pygame.mixer.init()
                SCARE_SOUND = pygame.mixer.Sound(SCARE_SOUND_PATH)
            else:
                print(f"⚠️  Jumpscare-lyd ikke funnet: {SCARE_SOUND_PATH}")
    except Exception as e:
        print(f"⚠️  Kunne ikke laste jumpscare-lyd: {e}")

def maybe_jumpscare(screen, is_last_map, clock):
    """
    Shows the scare and plays the scream. Returns the paused time (seconds),
    while ticking the same pygame.Clock so no giant dt accumulates.
    """
    if not SCARE_ENABLED:
        return 0.0
    if SCARE_ON_LAST_MAP_ONLY and not is_last_map:
        return 0.0
    if SCARE_IMG is None:
        return 0.0
    if random.random() >= SCARE_PROB:
        return 0.0

    # start audio
    ch = None
    try:
        if SCARE_SOUND_IS_MUSIC:
            if not pygame.mixer.get_init():
                pygame.mixer.init()
            pygame.mixer.music.load(SCARE_SOUND_PATH)
            pygame.mixer.music.play()
        elif SCARE_SOUND is not None:
            ch = SCARE_SOUND.play()
    except Exception:
        pass

    # show image fullscreen
    w, h = screen.get_size()
    try:
        img = pygame.transform.scale(SCARE_IMG, (w, h))
    except Exception:
        img = SCARE_IMG
    screen.blit(img, (0, 0))
    pygame.display.flip()

    # block while audio plays (or until duration), ticking the same clock
    start_ms = pygame.time.get_ticks()
    while True:
        # keep dt small by ticking during pause
        if clock is not None:
            clock.tick(FPS)

        elapsed = pygame.time.get_ticks() - start_ms
        if elapsed >= SCARE_DURATION_MS:
            break
        playing = False
        if SCARE_SOUND_IS_MUSIC:
            playing = pygame.mixer.music.get_busy()
        elif ch is not None:
            playing = ch.get_busy()
        if not playing:
            break
        for e in pygame.event.get():
            if e.type == pygame.QUIT:
                pygame.quit(); sys.exit(0)
        pygame.time.wait(10)
    if SCARE_SOUND_IS_MUSIC:
        try: pygame.mixer.music.stop()
        except Exception: pass
    return (pygame.time.get_ticks() - start_ms) / 1000.0


# ──────────────── MQTT helpers ────────────────
def mqtt_setup():
    if not MQTT_ENABLED or mqtt is None:
        return None

    # Create client with the *enum* for v2.x; fall back for v1.x
    try:
        client = mqtt.Client(
            protocol=mqtt.MQTTv311,
            callback_api_version=mqtt.CallbackAPIVersion.API_VERSION2  # <- enum, not 5
        )
    except Exception:
        # paho-mqtt < 2.0: no callback_api_version parameter
        client = mqtt.Client(protocol=mqtt.MQTTv311)

    try:
        client.connect(MQTT_BROKER, MQTT_PORT, keepalive=30)
        client.loop_start()
        return client
    except Exception as e:
        print(f"⚠️  MQTT-tilkobling feilet: {e}. Kjører uten MQTT.")
        return None


def mqtt_publish_score(client, event, payload):
    """Publish ONLY the numeric score to the configured topic."""
    if client is None or not MQTT_TOPIC_SCORE:
        return
    try:
        client.publish(MQTT_TOPIC_SCORE, str(payload), qos=1, retain=False)
    except Exception as e:
        print(f"⚠️  MQTT publish feilet: {e}")


# ──────────────── Game loop ────────────────
def game_loop(selected, campaign_info=None, mqtt_client=None):
    grid = load_map(selected)
    rows,cols = len(grid),len(grid[0])
    game_w, game_h = cols*TILE_SIZE, rows*TILE_SIZE
    win_w, win_h   = game_w + PANEL_WIDTH, game_h

    screen = pygame.display.set_mode((win_w, win_h))
    pygame.display.set_caption(f"TiltMaze – {selected}")

    start      = find_tile(grid,3)
    finish     = find_tile(grid,4)
    ball       = Ball(start, TILE_SIZE)
    checkpoint = start

    star_count = 0
    hole_count = 0
    t0 = pygame.time.get_ticks()/1000.0

    # UDP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(("0.0.0.0", UDP_PORT))
    sock.setblocking(False)
    tilt_x=tilt_y=0.0

    clock = pygame.time.Clock()
    font = pygame.font.Font(None, 24)

    gruppe_id       = campaign_info.get("gruppe_id") if campaign_info else None
    map_index       = campaign_info.get("map_index") if campaign_info else None
    per_map_scores  = campaign_info.get("per_map_scores") if campaign_info else None
    total_so_far    = campaign_info.get("total_so_far") if campaign_info else 0
    is_last_map     = campaign_info.get("is_last_map") if campaign_info else False

    while True:
        dt = clock.tick(FPS)/1000.0
        for e in pygame.event.get():
            if e.type==pygame.QUIT:
                pygame.quit(); sys.exit(0)

        # SECRET CHEAT: P+E+N+I+S
        keys = pygame.key.get_pressed()
        if (keys[pygame.K_p] and keys[pygame.K_e] and
            keys[pygame.K_n] and keys[pygame.K_i] and
            keys[pygame.K_s]):
            t_end = pygame.time.get_ticks()/1000.0 - t0
            base  = P0 * math.exp(-k * t_end)
            final = max(0, int(base + STAR_BONUS*star_count - HOLE_PENALTY*hole_count))
            stats = {"map": selected, "score": final, "time": round(t_end, 3),
                     "stars": star_count, "holes": hole_count}
            mqtt_publish_score(mqtt_client, "level_complete", stats["score"])
            sock.close()
            return {"replay": False, **stats}

        # Drain UDP (use only finite values)
        latest=None
        while True:
            try:
                data,_ = sock.recvfrom(8)
                latest = data
            except BlockingIOError:
                break
        if latest:
            tx_raw, ty_raw = struct.unpack("!ff", latest)
            if math.isfinite(tx_raw) and math.isfinite(ty_raw):
                tilt_x, tilt_y = tx_raw, ty_raw

        # Physics & collisions
        old = ball.pos.copy()
        ball.update(tilt_x, tilt_y, dt)

        # If physics blew up, reset to checkpoint
        if not (math.isfinite(ball.pos.x) and math.isfinite(ball.pos.y) and
                math.isfinite(ball.vel.x) and math.isfinite(ball.vel.y)):
            ball = Ball(checkpoint, TILE_SIZE)
            tilt_x = tilt_y = 0.0
            continue

        # X-axis collision
        tx,ty = int(ball.pos.x//TILE_SIZE), int(old.y//TILE_SIZE)
        if 0<=tx<cols and 0<=ty<rows and grid[ty][tx]==1:
            ball.pos.x = old.x
            ball.vel.x *= -RESTITUTION

        # Y-axis collision
        tx,ty = int(old.x//TILE_SIZE), int(ball.pos.y//TILE_SIZE)
        if 0<=tx<cols and 0<=ty<rows and grid[ty][tx]==1:
            ball.pos.y = old.y
            ball.vel.y *= -RESTITUTION

        # Cell under ball
        cx,cy = int(ball.pos.x//TILE_SIZE), int(ball.pos.y//TILE_SIZE)
        if 0<=cx<cols and 0<=cy<rows:
            cell = grid[cy][cx]
            if cell==2:  # hole
                hole_count += 1
                ball = Ball(checkpoint, TILE_SIZE)
                continue
            if cell==5:  # star
                star_count += 1
                grid[cy][cx] = 0
                # Jumpscare: pause timer and tick clock while it plays
                paused = maybe_jumpscare(screen, is_last_map, clock)
                if paused > 0:
                    t0 += paused
                    # also zero out velocities if you want *extra* safety:
                    # ball.vel.x = 0; ball.vel.y = 0
            if cell==6:  # checkpoint
                checkpoint = (cx,cy)
                grid[cy][cx] = 0
            if finish and (cx,cy)==finish:
                sock.close()
                t_end = pygame.time.get_ticks()/1000.0 - t0
                base  = P0 * math.exp(-k * t_end)
                final = max(0, int(base + STAR_BONUS*star_count - HOLE_PENALTY*hole_count))
                stats = {"map": selected, "score": final, "time": round(t_end, 3),
                         "stars": star_count, "holes": hole_count}
                mqtt_publish_score(mqtt_client, "level_complete", stats["score"])
                return {"replay": False, **stats}

        # Live score
        t_now     = pygame.time.get_ticks()/1000.0 - t0
        base_now  = P0 * math.exp(-k * t_now)
        current   = max(0, int(base_now + STAR_BONUS*star_count - HOLE_PENALTY*hole_count))

        # Draw game area
        screen.fill((20,20,20))
        render_grid(screen, grid)
        ball.draw(screen)

        # Draw side panel
        panel_x = game_w
        pygame.draw.rect(screen, PANEL_BG, (panel_x, 0, PANEL_WIDTH, win_h))

        lines = [
            f"Group: {gruppe_id or '-'}",
            f"Campaign: {map_index or '-'} / {CAMPAIGN_LEVELS}",
            f"Total: {total_so_far}",
            "",
            "Per-map score:",
        ]
        if per_map_scores and isinstance(per_map_scores, list):
            for i in range(CAMPAIGN_LEVELS):
                val = per_map_scores[i]
                disp = "-" if val is None else str(val)
                lines.append(f"Map{i+1}: {disp}")
        else:
            for i in range(CAMPAIGN_LEVELS):
                lines.append(f"Map{i+1}: -")

        lines += [
            "",
            f"Live score: {current}",
            f"Stars: {star_count}",
            f"Holes: {hole_count}",
            f"Tilt X: {tilt_x:+.2f}",
            f"Tilt Y: {tilt_y:+.2f}",
            f"CP: {checkpoint[0]},{checkpoint[1]}",
        ]

        y0 = 10
        for text in lines:
            if text == "":
                y0 += 10
                continue
            surf = font.render(text, True, TEXT_COLOR)
            screen.blit(surf, (panel_x+10, y0))
            y0 += 24

        pygame.display.flip()


# ──────────────── Campaign ────────────────
def resolve_campaign_maps():
    """Return map1.csv..map5.csv in order if they exist (skip missing)."""
    available = set(get_csv_files())
    ordered   = []
    for name in CAMPAIGN_MAP_NAMES:
        if name in available:
            ordered.append(name)
        else:
            print(f"⚠️  Kart mangler: {name} (hopper over)")
    if not ordered:
        print("❌ Ingen av kampanjekartene ble funnet.")
    return ordered

def run_campaign(mqtt_client, gruppe_id):
    selected_maps = resolve_campaign_maps()
    if not selected_maps:
        pygame.display.set_mode((800, 600))
        end_screen(pygame.display.get_surface(), 800, 600, 0)
        return

    total = 0
    per_map_scores = [None]*CAMPAIGN_LEVELS

    for i, m in enumerate(selected_maps, start=1):
        stats = game_loop(
            m,
            campaign_info={
                "gruppe_id": gruppe_id,
                "map_index": i,
                "per_map_scores": per_map_scores,
                "total_so_far": total,
                "is_last_map": (i == len(selected_maps))
            },
            mqtt_client=mqtt_client
        )
        total += stats["score"]
        per_map_scores[i-1] = stats["score"]

        if i < len(selected_maps):
            screen = pygame.display.set_mode((800, 600))
            cont = intermission_screen(
                screen, 800, 600,
                level_idx=i,
                level_score=stats["score"],
                total_score=total,
                next_map=selected_maps[i],
                gruppe_id=gruppe_id
            )
            if not cont:
                break

    # Publish TOTAL (just the number) before ending
    if mqtt_client:
        mqtt_publish_score(mqtt_client, "campaign_complete", total)
        pygame.time.wait(200)  # give it a moment to send

    # Confirmation screen and final screen
    screen = pygame.display.set_mode((800, 600))
    published_screen(screen, 800, 600, total_score=total, topic=MQTT_TOPIC_SCORE)
    end_screen(screen, 800, 600, total)


# ──────────────── Entrypoint ────────────────
def main():
    pygame.init()
    # Set a video mode BEFORE loading assets (needed by .convert())
    screen = pygame.display.set_mode((800, 600))
    # Load jumpscare assets after set_mode
    load_jumpscare_assets()

    gruppe_id = text_input_screen(screen, 800, 600, "Skriv inn gruppe_id:")
    if not gruppe_id:
        pygame.quit()
        sys.exit(0)

    global MQTT_TOPIC_SCORE
    MQTT_TOPIC_SCORE = f"zumo_car/penis/score/del2.1/{gruppe_id}"

    mqtt_client = mqtt_setup()

    if not start_screen(screen, 800, 600):
        pygame.quit()
        sys.exit(0)

    run_campaign(mqtt_client, gruppe_id)

    pygame.quit()
    sys.exit(0)


if __name__=="__main__":
    main()
