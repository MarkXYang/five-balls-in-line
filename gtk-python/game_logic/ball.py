class Ball:
    def __init__(self, color: str):
        if not isinstance(color, str) or not color:
            raise ValueError("Color must be a non-empty string")
        self.color = color

    def __repr__(self):
        return f"Ball('{self.color}')"

    def __eq__(self, other):
        if not isinstance(other, Ball):
            return NotImplemented
        return self.color == other.color

# Example usage (optional, for testing if runnable)
if __name__ == '__main__':
    try:
        ball1 = Ball("red")
        print(ball1)
        ball2 = Ball("red")
        print(ball1 == ball2)
        ball3 = Ball("blue")
        print(ball1 == ball3)
        # Test empty color
        # ball_empty = Ball("") # Expected ValueError
        # Test non-string color
        # ball_invalid = Ball(123) # Expected ValueError
    except ValueError as e:
        print(f"Error: {e}")
