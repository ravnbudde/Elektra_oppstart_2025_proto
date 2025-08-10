# src/maps/map_editor.py

import os
import csv
import pygame
import sys
import datetime

# ─── Configuration ────────────────────────────────────────
TILE_SIZE   = 15
PANEL_WIDTH = 200
FPS         = 60

# Define tile modes: key → (value, name, color)
TILE_MODES = {
    0: (0, "Empty",      (200,200,200)),
    1: (1, "Wall",       ( 50, 50, 50)),
    2: (2, "Hole",       (100,   0,  0)),
    3: (3, "Start",      (  0,200,  0)),
    4: (4, "Finish",     (  0,   0,200)),
    5: (5, "Star",       (255,215,  0)),
    6: (6, "Checkpoint", (255,   0,255)),
}
# ───────────────────────────────────────────────────────────

def load_template():
    here = os.path.dirname(os.path.abspath(__file__))
    template_dir = os.path.join(here, "template")
    if not os.path.isdir(template_dir):
        print(f"❌ Folder not found: {template_dir}")
        raise FileNotFoundError(template_dir)
    for fn in os.listdir(template_dir):
        if fn.lower().endswith(".csv"):
            path = os.path.join(template_dir, fn)
            break
    else:
        raise FileNotFoundError(f"No .csv in {template_dir}")
    grid = []
    with open(path, newline="") as f:
        for row in csv.reader(f):
            grid.append([int(cell) for cell in row])
    print(f"Loaded template from: {path}")
    return grid

def save_grid(grid):
    here = os.path.dirname(os.path.abspath(__file__))
    ts = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
    out_fn = f"map_{ts}.csv"
    out_fp = os.path.join(here, out_fn)
    with open(out_fp, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerows(grid)
    print(f"Saved edited map to: {out_fp}")

def main():
    grid = load_template()
    rows, cols = len(grid), len(grid[0])
    win_w = cols * TILE_SIZE + PANEL_WIDTH
    win_h = rows * TILE_SIZE

    pygame.init()
    screen = pygame.display.set_mode((win_w, win_h))
    pygame.display.set_caption("Map Editor – Modes: 0–6, LMB paint, RMB erase")
    clock = pygame.time.Clock()
    font = pygame.font.Font(None, 24)

    drawing = False
    draw_mode = 1  # default to Wall

    while True:
        for evt in pygame.event.get():
            if evt.type == pygame.QUIT:
                save_grid(grid)
                pygame.quit()
                sys.exit()

            elif evt.type == pygame.KEYDOWN:
                if pygame.K_0 <= evt.key <= pygame.K_6:
                    draw_mode = evt.key - pygame.K_0

            elif evt.type == pygame.MOUSEBUTTONDOWN:
                if evt.button == 1:
                    drawing = True
                elif evt.button == 3:
                    drawing = True
                    draw_mode = 0

            elif evt.type == pygame.MOUSEBUTTONUP:
                if evt.button in (1,3):
                    drawing = False

            elif evt.type == pygame.MOUSEMOTION and drawing:
                mx, my = evt.pos
                if mx < cols * TILE_SIZE:
                    gx, gy = mx // TILE_SIZE, my // TILE_SIZE
                    if 0 <= gx < cols and 0 <= gy < rows:
                        grid[gy][gx] = TILE_MODES[draw_mode][0]

        # Draw grid
        for y, row in enumerate(grid):
            for x, cell in enumerate(row):
                color = next(c for v,n,c in TILE_MODES.values() if v==cell)
                rect = pygame.Rect(x*TILE_SIZE, y*TILE_SIZE, TILE_SIZE, TILE_SIZE)
                pygame.draw.rect(screen, color, rect)

        # Draw UI panel
        panel_x = cols * TILE_SIZE
        pygame.draw.rect(screen, (40,40,40), (panel_x, 0, PANEL_WIDTH, win_h))
        screen.blit(font.render("Tile Modes:", True, (255,255,255)), (panel_x+10, 10))
        for idx, (key, (val,name,color)) in enumerate(TILE_MODES.items()):
            y_off = 40 + idx * 30
            text = f"{key}: {name}"
            col = (255,255,0) if key==draw_mode else (200,200,200)
            screen.blit(font.render(text, True, col), (panel_x+10, y_off))

        pygame.display.flip()
        clock.tick(FPS)

if __name__ == "__main__":
    main()
