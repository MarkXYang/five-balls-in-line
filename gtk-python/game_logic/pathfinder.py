from collections import deque
# GameGrid will be imported if type hinting is needed for an IDE,
# but for runtime, we assume it's passed correctly.
# from .grid import GameGrid

class Pathfinder:
    def __init__(self, game_grid): # game_grid: GameGrid
        self.game_grid = game_grid

    def can_reach(self, start_r: int, start_c: int, end_r: int, end_c: int) -> bool:
        if not self.game_grid:
            return False

        if not (0 <= start_r < self.game_grid.height and 0 <= start_c < self.game_grid.width and
                0 <= end_r < self.game_grid.height and 0 <= end_c < self.game_grid.width):
            return False # Start or end out of bounds

        if start_r == end_r and start_c == end_c:
            return True # Already at the destination

        # Target cell must be empty to move there
        if not self.game_grid.is_cell_empty(end_r, end_c):
            return False

        queue = deque([((start_r, start_c), [])])  # Store (position, path_taken)
        visited = set([(start_r, start_c)])

        # Directions: up, down, left, right
        directions = [(-1, 0), (1, 0), (0, -1), (0, 1)]

        while queue:
            (r, c), path = queue.popleft()

            for dr, dc in directions:
                next_r, next_c = r + dr, c + dc

                if next_r == end_r and next_c == end_c:
                    # Path found to the cell adjacent to the end, or to the end itself if it's empty
                    return True

                if (0 <= next_r < self.game_grid.height and
                        0 <= next_c < self.game_grid.width and
                        (self.game_grid.is_cell_empty(next_r, next_c)) and # Path must be on empty cells
                        (next_r, next_c) not in visited):

                    visited.add((next_r, next_c))
                    new_path = list(path) # Make a copy
                    new_path.append((next_r, next_c))
                    queue.append(((next_r, next_c), new_path))

        return False

if __name__ == '__main__':
    # Mock GameGrid and Ball for testing Pathfinder independently
    class MockBall:
        def __init__(self, color):
            self.color = color
        def __repr__(self):
            return f"B({self.color[0]})"

    class MockGameGrid:
        def __init__(self, width, height, layout_str: list[str]):
            self.width = width
            self.height = height
            self.grid = [[None for _ in range(width)] for _ in range(height)]
            for r, row_str in enumerate(layout_str):
                for c, char_val in enumerate(row_str):
                    if char_val != '.':
                        self.grid[r][c] = MockBall(char_val)

        def get_ball(self, r, c):
            return self.grid[r][c]

        def is_cell_empty(self, r, c):
            return 0 <= r < self.height and 0 <= c < self.width and self.grid[r][c] is None

        def print_grid(self):
            for r in range(self.height):
                print(" ".join([str(self.grid[r][c]) if self.grid[r][c] else '.' for c in range(self.width)]))

    # Test cases
    layout1 = [
        "R...",
        ".X.X", # X indicates a blocked cell (non-empty for pathfinder)
        "....",
        "X.X."
    ]
    # Path: (0,0) -> (0,1) -> (0,2) -> (0,3) -> (1,3) (blocked) -> (2,3) -> (2,2) -> (2,1) -> (2,0)
    # Let's make it simpler for can_reach
    grid_layout = [
        "S.X.", # S = start, . = empty, X = blocked
        "....",
        "..X.",
        "E..."  # E = end
    ]

    mock_grid_data = [[None for _ in range(4)] for _ in range(4)]
    # Convert grid_layout to a format MockGameGrid can use
    # For Pathfinder, 'S' and 'E' are just empty cells for path purposes,
    # the actual ball objects don't matter for path existence, only emptiness.

    path_test_layout = []
    start_pos, end_pos = None, None
    for r, row_str in enumerate(grid_layout):
        new_row = []
        for c, char_val in enumerate(row_str):
            if char_val == 'S':
                start_pos = (r,c)
                new_row.append('.') # Start is empty for pathing
            elif char_val == 'E':
                end_pos = (r,c)
                new_row.append('.') # End must be empty for pathing
            else:
                new_row.append(char_val)
        path_test_layout.append("".join(new_row))

    print("Pathfinder Test Grid:")
    test_gg = MockGameGrid(4, 4, path_test_layout)
    test_gg.print_grid()

    pf = Pathfinder(test_gg)

    if start_pos and end_pos:
        print(f"Testing path from {start_pos} to {end_pos}")
        can_reach_result = pf.can_reach(start_pos[0], start_pos[1], end_pos[0], end_pos[1])
        print(f"Pathfinder.can_reach({start_pos}, {end_pos}): {can_reach_result}") # Expected: True

        # Test with blocked path
        print(f"Testing path from {start_pos} to (0,2) (blocked)") # (0,2) is 'X'
        # To test this, we need to ensure (0,2) is NOT empty in the grid for can_reach.
        # The current can_reach checks if END is empty. If we want to test path TO a blocked cell
        # it will return false.
        # Let's test path to an empty cell that requires going around blocks.
        # e.g. S to (1,1)
        print(f"Pathfinder.can_reach({start_pos}, (1,1)): {pf.can_reach(start_pos[0], start_pos[1], 1, 1)}") # Expected: True

        # Test path to an unreachable empty cell
        blocked_layout = [
            "S.X.",
            "XXX.",
            "..XE", # E is now blocked off
            "...."
        ]
        bl_start_pos, bl_end_pos = None, None
        bl_path_test_layout = []
        for r, row_str in enumerate(blocked_layout):
            new_row = []
            for c, char_val in enumerate(row_str):
                if char_val == 'S': bl_start_pos = (r,c); new_row.append('.')
                elif char_val == 'E': bl_end_pos = (r,c); new_row.append('.')
                else: new_row.append(char_val)
            bl_path_test_layout.append("".join(new_row))

        print("\nBlocked Path Test Grid:")
        test_gg_blocked = MockGameGrid(4, 4, bl_path_test_layout)
        test_gg_blocked.print_grid()
        pf_blocked = Pathfinder(test_gg_blocked)
        if bl_start_pos and bl_end_pos:
             print(f"Testing path from {bl_start_pos} to {bl_end_pos} (expected False)")
             print(f"Pathfinder.can_reach({bl_start_pos}, {bl_end_pos}): {pf_blocked.can_reach(bl_start_pos[0], bl_start_pos[1], bl_end_pos[0], bl_end_pos[1])}")


    # Test case where end is not empty
    test_gg.grid[end_pos[0]][end_pos[1]] = MockBall('BlockedEnd') # Make end_pos non-empty
    print("\nTest Grid with Non-Empty End:")
    test_gg.print_grid()
    print(f"Testing path from {start_pos} to {end_pos} (now non-empty end)")
    can_reach_non_empty_end = pf.can_reach(start_pos[0], start_pos[1], end_pos[0], end_pos[1])
    print(f"Pathfinder.can_reach({start_pos}, {end_pos}): {can_reach_non_empty_end}") # Expected: False
