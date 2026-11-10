const csrfToken = document.body.dataset.csrfToken;
const basePath = "/oppstartsuka/scoreboard";
const loginForm = document.querySelector("#login-form");
const groupForm = document.querySelector("#group-form");
const scoreForm = document.querySelector("#score-form");
const columns = ["del1.1", "del1.2", "del2", "del3", "bonus"];
let selectedGroup = "";

async function api(path, options = {}) {
  const response = await fetch(path, {...options, headers: {"Content-Type": "application/json", "X-CSRF-Token": csrfToken, ...(options.headers || {})}});
  const data = await response.json().catch(() => ({}));
  if (!response.ok) throw new Error(data.error || "Noe gikk galt.");
  return data;
}

function showForm(form) {
  [loginForm, groupForm, scoreForm].forEach((item) => { item.hidden = item !== form; });
  form.querySelector("input")?.focus();
}

function setStatus(form, message, success = false) {
  const target = form.querySelector(".form-status");
  target.textContent = message;
  target.classList.toggle("success", success);
}

columns.forEach((column) => {
  const row = document.createElement("div");
  row.className = "score-field";
  row.innerHTML = `<label for="field-${column}">${column}:</label><input id="field-${column}" name="${column}" type="number" step="any"><label class="not-completed"><input type="checkbox" data-column="${column}"> Ikke gjennomført</label>`;
  document.querySelector("#score-fields").append(row);
});

document.querySelector("#score-fields").addEventListener("change", (event) => {
  if (!event.target.matches("[type=checkbox]")) return;
  const input = document.querySelector(`#field-${CSS.escape(event.target.dataset.column)}`);
  input.disabled = event.target.checked;
  if (event.target.checked) input.value = "";
});

loginForm.addEventListener("submit", async (event) => {
  event.preventDefault();
  try {
    await api(`${basePath}/api/login`, {method: "POST", body: JSON.stringify({password: loginForm.password.value})});
    loginForm.reset();
    setStatus(loginForm, "");
    showForm(groupForm);
  } catch (error) { setStatus(loginForm, error.message); }
});

groupForm.addEventListener("submit", async (event) => {
  event.preventDefault();
  selectedGroup = groupForm.group.value.trim();
  try {
    const data = await api(`${basePath}/api/groups/${encodeURIComponent(selectedGroup)}`);
    document.querySelector("#selected-group").textContent = `Gruppe: ${selectedGroup}`;
    columns.forEach((column) => {
      const input = document.querySelector(`#field-${CSS.escape(column)}`);
      const checkbox = document.querySelector(`[data-column="${column}"]`);
      const missing = data.values[column] === null;
      input.value = missing ? "" : data.values[column];
      input.disabled = missing;
      checkbox.checked = missing;
    });
    setStatus(groupForm, "");
    showForm(scoreForm);
  } catch (error) { setStatus(groupForm, error.message); }
});

document.querySelector("#score-back").addEventListener("click", async () => {
  try {
    await api(`${basePath}/api/logout`, {method: "POST", body: "{}"});
    selectedGroup = "";
    groupForm.reset();
    scoreForm.reset();
    setStatus(scoreForm, "");
    showForm(loginForm);
  } catch (error) { setStatus(scoreForm, error.message); }
});
scoreForm.addEventListener("submit", async (event) => {
  event.preventDefault();
  const values = {};
  columns.forEach((column) => {
    const checkbox = document.querySelector(`[data-column="${column}"]`);
    values[column] = checkbox.checked ? null : document.querySelector(`#field-${CSS.escape(column)}`).value;
  });
  try {
    await api(`${basePath}/api/groups/${encodeURIComponent(selectedGroup)}`, {method: "PUT", body: JSON.stringify(values)});
    setStatus(scoreForm, `Lagret for gruppe ${selectedGroup}`, true);
  } catch (error) { setStatus(scoreForm, error.message); }
});

api(`${basePath}/api/session`).then((state) => showForm(state.authenticated ? groupForm : loginForm));
