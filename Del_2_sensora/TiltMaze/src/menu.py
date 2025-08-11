import sys, pygame
from loadmap import get_csv_files

SCREEN_W, SCREEN_H = 800, 600
FONT_SIZE = 32
BG, FG, HL = (30,30,30), (200,200,200), (70,130,180)

def run_menu(screen, clock, font, files):
    selected = 0
    while True:
        for evt in pygame.event.get():
            if evt.type == pygame.QUIT:
                pygame.quit(); sys.exit()
            elif evt.type == pygame.KEYDOWN:
                if evt.key == pygame.K_UP:
                    selected = (selected - 1) % len(files)
                elif evt.key == pygame.K_DOWN:
                    selected = (selected + 1) % len(files)
                elif evt.key == pygame.K_RETURN:
                    return files[selected]

        screen.fill(BG)
        for i, name in enumerate(files):
            color = HL if i == selected else FG
            surf = font.render(name, True, color)
            screen.blit(surf, (50, 50 + i*(FONT_SIZE+10)))
        pygame.display.flip()
        clock.tick(30)

def main():
    pygame.init()
    screen = pygame.display.set_mode((SCREEN_W, SCREEN_H))
    clock  = pygame.time.Clock()
    font   = pygame.font.Font(None, FONT_SIZE)

    files = get_csv_files()
    if not files:
        print("❌ No CSV maps found in src/maps.")
        pygame.quit()
        return

    choice = run_menu(screen, clock, font, files)
    print(choice)
    pygame.quit()

if __name__=="__main__":
    main()
