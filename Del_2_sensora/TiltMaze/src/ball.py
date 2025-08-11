# src/ball.py
import pygame

# Physics constants
ACCEL_SCALE = 500.0    # px/sec² per unit tilt
FRICTION    = 0.98     # velocity multiplier per frame

BALL_COLOR  = (0,0,255)

class Ball:
    def __init__(self, start_tile, tile_size):
        """Place the ball in the center of the given (x,y) tile."""
        sx, sy = start_tile
        self.radius = tile_size * 0.4
        self.pos = pygame.math.Vector2(
            (sx + 0.5) * tile_size,
            (sy + 0.5) * tile_size
        )
        self.vel = pygame.math.Vector2(0, 0)

    def update(self, tilt_x, tilt_y, dt):
        """Integrate physics: accel → vel → pos, with friction."""
        accel = pygame.math.Vector2(tilt_x, tilt_y) * ACCEL_SCALE
        self.vel += accel * dt
        self.vel *= FRICTION
        self.pos += self.vel * dt

    def draw(self, screen):
        """Render the ball as a filled circle."""
        pygame.draw.circle(
            screen,
            BALL_COLOR,
            (int(self.pos.x), int(self.pos.y)),
            int(self.radius)
        )
