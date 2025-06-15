from .ball import Ball

class GameBoard:
    def __init__(self, rows=6, cols=7):
        self.rows = rows
        self.cols = cols
        # Initialize players as Ball objects
        self.player1 = Ball('R') # Red
        self.player2 = Ball('Y') # Yellow
        self.board = [[None for _ in range(cols)] for _ in range(rows)] # Store Ball objects or None
        self.current_player = self.player1
        self.game_over = False

    def drop_token(self, col):
        if self.game_over:
            return None
        if not (0 <= col < self.cols):
            raise ValueError("Invalid column")

        for r in range(self.rows - 1, -1, -1):
            if self.board[r][col] is None:
                self.board[r][col] = self.current_player
                return r, col
        return None

    def switch_player(self):
        if self.game_over:
            return
        self.current_player = self.player2 if self.current_player == self.player1 else self.player1

    def check_win(self, player_ball: Ball, row: int, col: int) -> bool:
        # Player_ball is the Ball object of the player who just moved
        # Check horizontal
        count = 0
        for c_offset in range(-3, 4):
            c_check = col + c_offset
            if 0 <= c_check < self.cols and self.board[row][c_check] == player_ball:
                count += 1
                if count == 4: self.game_over = True; return True
            else:
                count = 0

        # Check vertical
        count = 0
        for r_offset in range(-3, 4):
            r_check = row + r_offset
            if 0 <= r_check < self.rows and self.board[r_check][col] == player_ball:
                count += 1
                if count == 4: self.game_over = True; return True
            else:
                count = 0

        # Check positive diagonal (\)
        count = 0
        for i in range(-3, 4):
            r_check, c_check = row + i, col + i
            if 0 <= r_check < self.rows and 0 <= c_check < self.cols and \
               self.board[r_check][c_check] == player_ball:
                count += 1
                if count == 4: self.game_over = True; return True
            else:
                count = 0

        # Check negative diagonal (/)
        count = 0
        for i in range(-3, 4):
            r_check, c_check = row - i, col + i
            if 0 <= r_check < self.rows and 0 <= c_check < self.cols and \
               self.board[r_check][c_check] == player_ball:
                count += 1
                if count == 4: self.game_over = True; return True
            else:
                count = 0

        return False

    def is_full(self):
        if self.game_over:
            return False
        full = all(self.board[0][c] is not None for c in range(self.cols))
        if full:
            self.game_over = True
        return full

    def reset_board(self):
        self.board = [[None for _ in range(self.cols)] for _ in range(self.rows)]
        self.current_player = self.player1 # Reset to player 1
        self.game_over = False
