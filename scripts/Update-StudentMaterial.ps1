# Hent nyeste utdelingsmateriale uten å lagre en ny kildekopi i arrangørrepoet.
[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent $PSScriptRoot
$studentPath = Join-Path $repoRoot 'student-materiale'
$studentUrl = 'https://github.com/Adriaeik/BIAIS_Oppstartsuka_25.git'

function Invoke-StudentGit {
    param([string[]]$GitArgs)
    & git @GitArgs
    if ($LASTEXITCODE -ne 0) {
        throw "Git-kommandoen feilet. Ingen reset eller overskriving blir utført."
    }
}

if (-not (Get-Command git -ErrorAction SilentlyContinue)) {
    throw 'Git må være installert og tilgjengelig i PATH.'
}

if (-not (Test-Path -LiteralPath $studentPath)) {
    Invoke-StudentGit -GitArgs @('clone', '--branch', 'main', '--single-branch', $studentUrl, $studentPath)
} else {
    if (-not (Test-Path -LiteralPath (Join-Path $studentPath '.git'))) {
        throw 'student-materiale finnes, men er ikke en egen Git-klone. Mappen endres ikke.'
    }
    $origin = Invoke-StudentGit -GitArgs @('-C', $studentPath, 'remote', 'get-url', 'origin')
    if ($origin.TrimEnd('/') -notin @($studentUrl, $studentUrl.Substring(0, $studentUrl.Length - 4))) {
        throw 'student-materiale har en annen origin enn forventet. Mappen endres ikke.'
    }
    $changes = Invoke-StudentGit -GitArgs @('-C', $studentPath, 'status', '--porcelain', '--untracked-files=normal')
    if ($changes) {
        throw 'Utdelingsrepoet har lokale endringer. Commit eller flytt dem før oppdatering.'
    }
    $branch = Invoke-StudentGit -GitArgs @('-C', $studentPath, 'branch', '--show-current')
    if ($branch -ne 'main') {
        throw 'Utdelingsrepoet står ikke på main. Bytt branch selv før oppdatering.'
    }
    # Kan hente remote-objekter, men fast-forward er eneste tillatte endring i arbeidskopien.
    Invoke-StudentGit -GitArgs @('-C', $studentPath, 'fetch', 'origin', 'main')
    $localCommits = Invoke-StudentGit -GitArgs @('-C', $studentPath, 'rev-list', '--count', 'FETCH_HEAD..HEAD')
    if ($localCommits -ne '0') {
        throw 'Utdelingsrepoet har lokale commits som ikke finnes på origin/main. Mappen endres ikke.'
    }
    Invoke-StudentGit -GitArgs @('-C', $studentPath, 'merge', '--ff-only', 'FETCH_HEAD')
}

Write-Host 'Utdelingsmaterialet er klart. Noter denne revisjonen for arrangementet:'
Invoke-StudentGit -GitArgs @('-C', $studentPath, 'log', '-1', '--format=%H %cs %s')
Write-Host "MATLAB: åpne $studentPath og kjør startup.m."
