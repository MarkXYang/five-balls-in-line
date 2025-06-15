import random
from typing import Optional, List, Tuple # Added List and Tuple for other hints

# Conditionally import Ball for GameGrid internal use.
# If run as a script and relative import fails, __main__ will use a MockBall via BALL_CLASS_FOR_GRID.
try:
    from .ball import Ball
    BALL_CLASS_FOR_GRID = Ball
except ImportError:
    # This block will be executed if "from .ball import Ball" fails,
    # e.g., when running "python grid.py" directly.
    if __name__ == '__main__':
        # Define a MockBall that GameGrid can use if the real one isn't available.
        class MockBallForGridTesting:
            def __init__(self, color: str):
                if not isinstance(color, str) or not color:
                    raise ValueError("Color must be a non-empty string")
                self.color = color
            def __repr__(self):
                return f"Ball('{self.color}')" # Keep repr similar for test output consistency
            def __eq__(self, other):
                return isinstance(other, MockBallForGridTesting) and self.color == other.color
        BALL_CLASS_FOR_GRID = MockBallForGridTesting
    else:
        # If not in __main__ (i.e., being imported by another module in the package)
        # and the relative import failed, then it's a real problem.
        print("Error: Could not import .ball from grid.py when imported as a module.")
        raise

class GameGrid:
    def __init__(self, width: int = 9, height: int = 9):
        if not (isinstance(width, int) and width > 0 and isinstance(height, int) and height > 0):
            raise ValueError("Width and height must be positive integers.")
        self.width = width
        self.height = height
        self.grid = [[None for _ in range(width)] for _ in range(height)]
        self.available_colors = ["red", "green", "blue", "yellow", "purple", "pink", "brown", "turquoise"]

    def get_ball(self, r: int, c: int) -> Optional['Ball']:
        if 0 <= r < self.height and 0 <= c < self.width:
            return self.grid[r][c]
        return None

    def place_ball(self, r: int, c: int, ball_color: str) -> Optional['Ball']:
        if not (0 <= r < self.height and 0 <= c < self.width):
            # print(f"Error: Position ({r}, {c}) is out of bounds.")
            return None
        if self.grid[r][c] is None:
            if ball_color not in self.available_colors:
                # print(f"Warning: Color {ball_color} not in available_colors. Still placing.")
                pass
            new_ball = BALL_CLASS_FOR_GRID(ball_color) # Use the determined Ball class
            self.grid[r][c] = new_ball
            return new_ball
        # print(f"Error: Position ({r}, {c}) is already occupied by {self.grid[r][c]}.")
        return None

    def remove_ball(self, r: int, c: int) -> Optional['Ball']:
        if 0 <= r < self.height and 0 <= c < self.width:
            ball = self.grid[r][c]
            self.grid[r][c] = None
            return ball
        return None

    def is_cell_empty(self, r: int, c: int) -> bool:
        if 0 <= r < self.height and 0 <= c < self.width:
            return self.grid[r][c] is None
        # Out of bounds is not considered 'empty' in a way that's useful for placement
        return False

    def get_empty_cells(self) -> List[Tuple[int, int]]:
        empty_cells = []
        for r in range(self.height):
            for c in range(self.width):
                if self.grid[r][c] is None:
                    empty_cells.append((r, c))
        return empty_cells

    def add_random_balls(self, count: int) -> list[tuple[int, int]]:
        empty_cells = self.get_empty_cells()
        random.shuffle(empty_cells)

        added_balls_positions: List[Tuple[int, int]] = []
        num_to_add = min(count, len(empty_cells))

        if not self.available_colors:
            # print("Error: No available colors to choose from.")
            return [] # Return empty list, already correctly typed

        for i in range(num_to_add):
            r_pos, c_pos = empty_cells[i] # Renamed to avoid conflict with r, c loop vars
            random_color = random.choice(self.available_colors)
            if self.place_ball(r_pos, c_pos, random_color):
                added_balls_positions.append((r_pos, c_pos))

        return added_balls_positions

    def is_full(self) -> bool:
        return not any(None in row for row in self.grid)

    def reset(self):
        self.grid = [[None for _ in range(self.width)] for _ in range(self.height)]

    def __repr__(self):
        return f"GameGrid(width={self.width}, height={self.height})"

if __name__ == '__main__':
    # This __main__ block is for testing GameGrid.
    # Since GameGrid uses BALL_CLASS_FOR_GRID internally, and BALL_CLASS_FOR_GRID
    # is set to MockBallForGridTesting when this script is run directly and .ball import fails,
    # these tests will use the mocked Ball version.

    # Example Usage
    grid = GameGrid(5, 5)
    print(grid)
    grid.place_ball(0, 0, "red")
    grid.place_ball(1, 1, "blue")
    print(f"Ball at (0,0): {grid.get_ball(0,0)}")
    print(f"Is (0,1) empty? {grid.is_cell_empty(0,1)}")

    initial_empty_count = len(grid.get_empty_cells())
    print(f"Initial empty cells: {initial_empty_count}")

    added_pos = grid.add_random_balls(3)
    print(f"Added 3 random balls at: {added_pos}")

    current_empty_count = len(grid.get_empty_cells())
    print(f"Empty cells after adding: {current_empty_count}")
    # Expected: initial_empty_count - 3 (if enough space and colors)

    for r_idx, row in enumerate(grid.grid):
        print(f"Row {r_idx}: {[ball.color if ball else '.' for ball in row]}")

    print(f"Is grid full? {grid.is_full()}")

    grid.remove_ball(0,0)
    print(f"Ball at (0,0) after removal: {grid.get_ball(0,0)}")

    grid.reset()
    print(f"Is grid full after reset? {grid.is_full()}")
    print(f"Empty cells after reset: {len(grid.get_empty_cells())}")
