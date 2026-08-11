const basePath = "/oppstartsuka/scoreboard";
const scoreRows = document.querySelector("#score-rows");
const emptyState = document.querySelector("#empty-state");
const connectionStatus = document.querySelector("#connection-status");

function escapeText(value) {
  const span = document.createElement("span");
  span.textContent = value;
  return span.innerHTML;
}

async function api(path) {
  const response = await fetch(path);
  const data = await response.json().catch(() => ({}));
  if (!response.ok) throw new Error(data.error || "Noe gikk galt.");
  return data;
}

function taskCell(task) {
  const result = task.raw === null ? "-" : Number(task.raw).toFixed(2);
  return `${result} (${task.points.toFixed(2)}%)`;
}

async function refreshScores() {
  try {
    const {scores} = await api(`${basePath}/api/scores`);
    scoreRows.replaceChildren();
    scores.forEach((score, index) => {
      const row = document.createElement("tr");
      const medalName = ["gold", "silver", "bronze"][index];
      const medal = medalName ? `<img src="${basePath}/static/pic/${medalName}.png" alt="${index + 1}. plass">` : "";
      row.innerHTML = `<td><span class="rank">${medal}</span>${escapeText(score.group)}</td><td>${score.total.toFixed(2)}</td><td>${taskCell(score.tasks["del1.1"])}</td><td>${taskCell(score.tasks["del1.2"])}</td><td>${taskCell(score.tasks.del2)}</td><td>${taskCell(score.tasks.del3)}</td>`;
      scoreRows.append(row);
    });
    emptyState.hidden = scores.length !== 0;
    connectionStatus.textContent = `Oppdatert ${new Date().toLocaleTimeString("no-NO")}`;
  } catch (_error) {
    connectionStatus.textContent = "Kunne ikke hente resultater";
  }
}

refreshScores();
setInterval(refreshScores, 1000);
