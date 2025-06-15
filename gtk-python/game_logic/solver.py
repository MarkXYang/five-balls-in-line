# Assuming Ball and GameGrid might be imported for type hints if needed by an IDE
# from .ball import Ball
# from .grid import GameGrid
from typing import List, Tuple, Set

class Solver:
    def __init__(self, game_grid): # game_grid: GameGrid
        self.game_grid = game_grid

    def _check_direction(self, r: int, c: int, dr: int, dc: int, color: str) -> List[Tuple[int, int]]:
        """
        Checks one specific direction (e.g., dr=0, dc=1 for horizontal right)
        and its opposite (dr=0, dc=-1 for horizontal left) starting from (r,c).
        Returns a list of coordinates of all balls forming a continuous line of the same color
        through (r,c) along this axis.
        Includes (r,c) itself.
        """
        line_cells: List[Tuple[int, int]] = [(r, c)]

        # Check in the positive direction (dr, dc)
        current_r, current_c = r + dr, c + dc
        while (0 <= current_r < self.game_grid.height and
               0 <= current_c < self.game_grid.width):
            ball = self.game_grid.get_ball(current_r, current_c)
            if ball and ball.color == color:
                line_cells.append((current_r, current_c))
                current_r += dr
                current_c += dc
            else:
                break

        # Check in the negative direction (-dr, -dc)
        # (Only if dr or dc is not zero, to avoid double counting for stationary check or re-checking origin)
        if not (dr == 0 and dc == 0):
            current_r, current_c = r - dr, c - dc
            while (0 <= current_r < self.game_grid.height and
                   0 <= current_c < self.game_grid.width):
                ball = self.game_grid.get_ball(current_r, current_c)
                if ball and ball.color == color:
                    line_cells.append((current_r, current_c))
                    current_r -= dr
                    current_c -= dc
                else:
                    break
        return line_cells

    def find_lines_at(self, r: int, c: int, min_length: int = 5) -> List[Tuple[int, int]]:
        """
        Checks for lines of min_length or more involving the ball at (r, c).
        This is the optimized approach: only check lines related to the last move/placement.
        Returns a list of (r, c) tuples of all balls that form part of any found line(s).
        Uses a set to ensure uniqueness if a ball is part of multiple lines (e.g., intersection).
        """
        if not self.game_grid:
            return []

        start_ball = self.game_grid.get_ball(r, c)
        if not start_ball:
            return [] # No ball at the specified starting position

        ball_color = start_ball.color

        # Directions to check: Horizontal, Vertical, Diagonal Down-Right, Diagonal Up-Right
        # Each call to _check_direction will scan both ways along that axis.
        directions = [
            (0, 1),  # Horizontal (checks left and right)
            (1, 0),  # Vertical (checks up and down)
            (1, 1),  # Diagonal \ (checks top-left and bottom-right)
            (1, -1)  # Diagonal / (checks bottom-left and top-right)
        ]

        all_line_cells: Set[Tuple[int, int]] = set()

        for dr, dc in directions:
            line_on_axis = self._check_direction(r, c, dr, dc, ball_color)
            if len(line_on_axis) >= min_length:
                for cell_pos in line_on_axis:
                    all_line_cells.add(cell_pos)

        return list(all_line_cells)

if __name__ == '__main__':
    # Mock classes for testing Solver independently
    class MockBall:
        def __init__(self, color: str):
            self.color = color
        def __repr__(self):
            return f"B({self.color[0].upper()})"

    class MockGameGrid:
        def __init__(self, width: int, height: int, layout: List[str]):
            self.width = width
            self.height = height
            self.grid = [[None for _ in range(width)] for _ in range(height)]
            for r_idx, row_str in enumerate(layout):
                for c_idx, char_val in enumerate(row_str):
                    if char_val != '.':
                        self.grid[r_idx][c_idx] = MockBall(char_val)

        def get_ball(self, r: int, c: int):
            if 0 <= r < self.height and 0 <= c < self.width:
                return self.grid[r][c]
            return None

        def print_grid(self, highlight_cells: List[Tuple[int,int]] = None):
            if highlight_cells is None:
                highlight_cells = []
            for r_idx in range(self.height):
                row_display = []
                for c_idx in range(self.width):
                    cell = self.grid[r_idx][c_idx]
                    is_highlighted = (r_idx, c_idx) in highlight_cells
                    if cell:
                        display_char = str(cell)
                        if is_highlighted:
                            row_display.append(f"*{display_char}*")
                        else:
                            row_display.append(f" {display_char} ")
                    else:
                        if is_highlighted: # Should not happen for empty cells with current logic
                            row_display.append("*.*")
                        else:
                            row_display.append(" . ")
                print("".join(row_display))
            print("-" * (self.width * 3))


    # Test Cases
    print("Solver Test Case 1: Horizontal Line")
    layout1 = [
        ".......",
        ".RRRRR.", # Line of 5 Reds, check from (1,3) which is 'R'
        ".......",
    ]
    grid1 = MockGameGrid(7, 3, layout1)
    solver1 = Solver(grid1)
    grid1.print_grid()
    lines_found = solver1.find_lines_at(1, 3, min_length=5)
    print(f"Checking lines at (1,3) for min_length=5. Found: {sorted(lines_found)}")
    grid1.print_grid(highlight_cells=lines_found)
    # Expected: [(1,1), (1,2), (1,3), (1,4), (1,5)]

    print("\nSolver Test Case 2: Vertical Line")
    layout2 = [
        ".B.",
        ".B.",
        ".B.",
        ".B.",
        ".B.", # Line of 5 Blues, check from (2,1)
        "..."
    ]
    grid2 = MockGameGrid(3, 6, layout2)
    solver2 = Solver(grid2)
    grid2.print_grid()
    lines_found_v = solver2.find_lines_at(2, 1, min_length=5)
    print(f"Checking lines at (2,1) for min_length=5. Found: {sorted(lines_found_v)}")
    grid2.print_grid(highlight_cells=lines_found_v)
     # Expected: [(0,1), (1,1), (2,1), (3,1), (4,1)]

    print("\nSolver Test Case 3: Diagonal Line (\)")
    layout3 = [
        "G....",
        ".G...",
        "..G..", # Check from (2,2)
        "...G.",
        "....G",
        "....."
    ]
    grid3 = MockGameGrid(5, 6, layout3)
    solver3 = Solver(grid3)
    grid3.print_grid()
    lines_found_d1 = solver3.find_lines_at(2, 2, min_length=5)
    print(f"Checking lines at (2,2) for min_length=5. Found: {sorted(lines_found_d1)}")
    grid3.print_grid(highlight_cells=lines_found_d1)
    # Expected: [(0,0), (1,1), (2,2), (3,3), (4,4)]

    print("\nSolver Test Case 4: Diagonal Line (/)")
    layout4 = [
        "....Y",
        "...Y.",
        "..Y..", # Check from (2,2)
        ".Y...",
        "Y....",
        "....."
    ]
    grid4 = MockGameGrid(5, 6, layout4)
    solver4 = Solver(grid4)
    grid4.print_grid()
    lines_found_d2 = solver4.find_lines_at(2, 2, min_length=5)
    print(f"Checking lines at (2,2) for min_length=5. Found: {sorted(lines_found_d2)}")
    grid4.print_grid(highlight_cells=lines_found_d2)
    # Expected: [(0,4), (1,3), (2,2), (3,1), (4,0)]

    print("\nSolver Test Case 5: Intersection of lines")
    layout5 = [
        ".B.",
        "BBBBB", # H line
        ".B.",   # V line intersects H line at (1,2)
        ".B.",
        ".B."
    ]
    grid5 = MockGameGrid(5, 5, layout5)
    solver5 = Solver(grid5)
    grid5.print_grid()
    # Check from the intersection point (1,2), which is 'B'
    lines_found_intersect = solver5.find_lines_at(1, 2, min_length=3) # min_length=3 for easier check
    print(f"Checking lines at (1,2) for min_length=3. Found: {sorted(lines_found_intersect)}")
    grid5.print_grid(highlight_cells=lines_found_intersect)
    # Expected: [(0,2), (1,0), (1,1), (1,2), (1,3), (1,4), (2,2), (3,2), (4,2)] - unique points

    print("\nSolver Test Case 6: No line")
    layout6 = [
        "R.R",
        ".R.",
        "R.R"
    ]
    grid6 = MockGameGrid(3,3, layout6)
    solver6 = Solver(grid6)
    grid6.print_grid()
    lines_found_none = solver6.find_lines_at(1,1, min_length=5)
    print(f"Checking lines at (1,1) for min_length=5. Found: {sorted(lines_found_none)}")
    grid6.print_grid(highlight_cells=lines_found_none)
    # Expected: []

    print("\nSolver Test Case 7: Line shorter than min_length")
    layout7 = ["RRRR..."] # Line of 4
    grid7 = MockGameGrid(7,1, layout7)
    solver7 = Solver(grid7)
    grid7.print_grid()
    lines_found_short = solver7.find_lines_at(0,1, min_length=5)
    print(f"Checking lines at (0,1) for min_length=5. Found: {sorted(lines_found_short)}")
    # Expected: []

# Ensure to add from typing import List, Tuple, Set at the top if not already present
# for the main class if these type hints are used there. The __main__ block is self-contained.
# The Solver class itself uses List, Tuple, Set from typing.
# The prompt had them, ensuring they are in the created file.
