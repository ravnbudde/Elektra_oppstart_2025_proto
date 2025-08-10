import sys
import socket
import struct
import pygame
import math
from loadmap import get_csv_files, load_map
from menu   import run_menu
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
PANEL_WIDTH = 200
PANEL_BG    = (40, 40, 40)
TEXT_COLOR  = (255, 255, 255)

COLORS = {
    0: (200,200,200),
    1: ( 50, 50, 50),
    2: (100,  0,  0),
    3: (  0,200,  0),
    4: (  0,  0,200),
    5: (255,215,  0),
    6: (255,  0,255),
}

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
    l1 = big.render("🎉 You Win! 🎉", True, TEXT_COLOR)
    l2 = small.render(f"Final Score: {final_score}", True, TEXT_COLOR)
    l3 = small.render("Press R to Replay or Q to Quit", True, (200,200,200))
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
            if e.type==pygame.KEYDOWN:
                if e.key==pygame.K_r: return True
                if e.key==pygame.K_q: return False
        pygame.time.wait(100)

def game_loop(selected):
    # Load map & init
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

    # UDP setup
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(("0.0.0.0", UDP_PORT))
    sock.setblocking(False)
    tilt_x=tilt_y=0.0

    clock = pygame.time.Clock()
    font = pygame.font.Font(None, 24)

    while True:
        dt = clock.tick(FPS)/1000.0
        for e in pygame.event.get():
            if e.type==pygame.QUIT:
                pygame.quit(); sys.exit(0)

        # Drain UDP
        latest=None
        while True:
            try: data,_ = sock.recvfrom(8); latest=data
            except BlockingIOError: break
        if latest:
            tilt_x,tilt_y = struct.unpack("!ff", latest)

        # Physics & collisions
        old = ball.pos.copy()
        ball.update(tilt_x, tilt_y, dt)

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
            if cell==6:  # checkpoint
                checkpoint = (cx,cy)
                grid[cy][cx] = 0
            if finish and (cx,cy)==finish:
                sock.close()
                # final scoring
                t_end = pygame.time.get_ticks()/1000.0 - t0
                base  = P0 * math.exp(-k * t_end)
                final = max(0, int(base + STAR_BONUS*star_count - HOLE_PENALTY*hole_count))
                return end_screen(screen, win_w, win_h, final)

        # Compute current score
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
            f"Score: {current}",
            f"Stars: {star_count}",
            f"Holes: {hole_count}",
            f"Tilt X: {tilt_x:+.2f}",
            f"Tilt Y: {tilt_y:+.2f}",
            f"CP: {checkpoint[0]},{checkpoint[1]}"
        ]
        for i, text in enumerate(lines):
            surf = font.render(text, True, TEXT_COLOR)
            screen.blit(surf, (panel_x+10, 10 + i*30))

        pygame.display.flip()

    sock.close()
    return False

def main():
    pygame.init()
    files = get_csv_files()
    if not files:
        print("❌ No maps in src/maps!"); sys.exit(1)

    # Map selection
    screen = pygame.display.set_mode((800,600))
    clock  = pygame.time.Clock()
    font   = pygame.font.Font(None,24)
    selected = run_menu(screen, clock, font, files)

    # Game & replay loop
    while game_loop(selected):
        pass

    pygame.quit()
    sys.exit(0)

if __name__=="__main__":
    main()
