import gi
gi.require_version("Gtk", "3.0")
from gi.repository import Gtk, Gdk
from game_logic.game_board import GameBoard # Ensure this path is correct

class GameWindow(Gtk.ApplicationWindow):
    def __init__(self, application):
        super().__init__(application=application, title="Connect Four")

        self.game_board = GameBoard()
        self.set_default_size(700, 700) # Adjusted for potentially larger board/buttons
        self.set_position(Gtk.WindowPosition.CENTER)
        self.set_border_width(10)

        # Main vertical box
        vbox = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=10)
        self.add(vbox)

        # Create the game board drawing area
        self.drawing_area = Gtk.DrawingArea()
        self.drawing_area.set_size_request(600, 500) # Width, Height for 7x6 board
        self.drawing_area.connect("draw", self.on_draw)
        # Add a frame around the drawing area for better visual separation
        frame = Gtk.Frame(label="Board")
        frame.add(self.drawing_area)
        vbox.pack_start(frame, True, True, 0)

        # Grid for column buttons
        self.button_grid = Gtk.Grid(column_spacing=10, row_spacing=5)
        self.button_grid.set_halign(Gtk.Align.CENTER)
        vbox.pack_start(self.button_grid, False, False, 0)

        self.column_buttons = []
        for col in range(self.game_board.cols):
            button = Gtk.Button(label=f"Drop Col {col+1}")
            button.connect("clicked", self.on_column_selected, col)
            self.button_grid.attach(button, col, 0, 1, 1)
            self.column_buttons.append(button)

        # Status Label
        self.status_label = Gtk.Label(label=f"Player {self.game_board.current_player.color}'s turn") # Access .color
        self.status_label.get_style_context().add_class("status-label") # For CSS
        vbox.pack_start(self.status_label, False, False, 10) # Add padding

        # Restart Button
        self.restart_button = Gtk.Button(label="Restart Game")
        self.restart_button.set_name("restart_button") # For CSS styling
        self.restart_button.get_style_context().add_class("restart-button") # For CSS
        self.restart_button.connect("clicked", self.on_restart_clicked)
        self.restart_button.set_halign(Gtk.Align.CENTER)
        vbox.pack_start(self.restart_button, False, False, 5)

        self.show_all()

    def on_column_selected(self, button, column):
        if "wins!" in self.status_label.get_text() or "draw!" in self.status_label.get_text():
            return # Game is over

        player_ball_dropped = self.game_board.current_player # This is the Ball object that will be dropped
        dropped_pos = self.game_board.drop_token(column)

        if dropped_pos:
            row, col = dropped_pos
            self.drawing_area.queue_draw()  # Trigger redraw

            if self.game_board.check_win(player_ball_dropped, row, col): # Pass the Ball object
                self.update_status(f"Player {player_ball_dropped.color} wins!") # Access .color
                self.disable_column_buttons()
            elif self.game_board.is_full():
                self.update_status("It's a draw!")
                self.disable_column_buttons()
            else:
                self.game_board.switch_player()
                self.update_status(f"Player {self.game_board.current_player.color}'s turn") # Access .color
        else:
            # Briefly indicate column is full
            original_text = self.status_label.get_text()
            self.update_status("Column is full. Try another.")
            # Optional: Revert message after a delay, or rely on user to try another column
            # GLib.timeout_add(2000, self.update_status, original_text)


    def on_draw(self, widget, cr):
        context = widget.get_style_context()
        width = widget.get_allocated_width()
        height = widget.get_allocated_height()

        # Background
        bg_color = context.get_background_color(Gtk.StateFlags.NORMAL)
        cr.set_source_rgba(bg_color.red, bg_color.green, bg_color.blue, bg_color.alpha)
        cr.paint()

        cell_width = width / self.game_board.cols
        cell_height = height / self.game_board.rows
        # Radius should be slightly less than half the smaller of cell_width or cell_height
        radius = min(cell_width, cell_height) / 2 - 5


        for r_idx in range(self.game_board.rows):
            for c_idx in range(self.game_board.cols):
                # Calculate center of the circle for the slot
                x_center = c_idx * cell_width + cell_width / 2
                y_center = r_idx * cell_height + cell_height / 2

                # Draw empty slot (e.g., light grey or a hole color)
                cr.save()
                # Use a specific color for empty slots from CSS or default
                empty_slot_color = Gdk.RGBA(0.85, 0.85, 0.85, 1.0) # Light Grey
                style_context = self.get_style_context()
                # TODO: Define 'empty-slot' color in CSS and retrieve it here if desired
                # path = Gtk.WidgetPath()
                # path.append_type(Gtk.DrawingArea) # Or a more specific type if styled
                # path.add_class("board-slot-empty")
                # Gtk.StyleContext.lookup_color(style_context, "empty_slot_color_name", empty_slot_color)

                cr.set_source_rgba(empty_slot_color.red, empty_slot_color.green, empty_slot_color.blue, empty_slot_color.alpha)
                cr.arc(x_center, y_center, radius, 0, 2 * 3.14159265) # 2*pi
                cr.fill()
                cr.restore()

                ball_on_board = self.game_board.board[r_idx][c_idx] # This is a Ball object or None
                if ball_on_board:
                    token_gdk_color = Gdk.RGBA() # Default to black if something goes wrong
                    if ball_on_board.color == 'R':
                        token_gdk_color.parse("red")
                    elif ball_on_board.color == 'Y':
                        token_gdk_color.parse("yellow")
                    # Add more colors if Ball class supports them, e.g. ball_on_board.color == 'B'
                    # else: token_gdk_color.parse(ball_on_board.color.lower()) # If Ball.color stores CSS color names

                    cr.save()
                    cr.set_source_rgba(token_gdk_color.red, token_gdk_color.green, token_gdk_color.blue, token_gdk_color.alpha)
                    cr.arc(x_center, y_center, radius, 0, 2 * 3.14159265)
                    cr.fill()
                    cr.restore()

    def disable_column_buttons(self):
        for button in self.column_buttons:
            button.set_sensitive(False)

    def enable_column_buttons(self):
        for button in self.column_buttons:
            button.set_sensitive(True)

    def on_restart_clicked(self, button):
        self.game_board.reset_board()
        self.enable_column_buttons()
        self.update_status(f"Player {self.game_board.current_player.color}'s turn") # Access .color
        self.drawing_area.queue_draw()

    def update_status(self, message):
        self.status_label.set_text(message)
