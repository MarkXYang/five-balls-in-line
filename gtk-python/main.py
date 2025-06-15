import gi
gi.require_version("Gtk", "3.0")
from gi.repository import Gtk, Gdk
import os
import sys

# Ensure game_logic and ui directories are in Python's search path
# This is usually handled by __init__.py files and running from the correct directory,
# but can be made more explicit if needed.
# current_dir = os.path.dirname(os.path.abspath(__file__))
# sys.path.append(os.path.join(current_dir, 'ui'))
# sys.path.append(os.path.join(current_dir, 'game_logic'))

from ui.game_window import GameWindow

class ConnectFourApp(Gtk.Application):
    def __init__(self):
        super().__init__(application_id="com.example.connectfour",
                         flags=gi.Gio.ApplicationFlags.FLAGS_NONE)

    def do_startup(self):
        Gtk.Application.do_startup(self)

        css_provider = Gtk.CssProvider()
        css_file_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), "assets", "style.css")

        if os.path.exists(css_file_path):
            try:
                css_provider.load_from_path(css_file_path)
                screen = Gdk.Screen.get_default()
                Gtk.StyleContext.add_provider_for_screen(screen, css_provider, Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION)
                print(f"CSS loaded from {css_file_path}")
            except Exception as e:
                print(f"Error loading CSS from {css_file_path}: {e}")
        else:
            print(f"CSS file not found at {css_file_path}. Styles will not be applied.")


    def do_activate(self):
        win = self.props.active_window
        if not win:
            win = GameWindow(application=self)
        win.present()

def main():
    app = ConnectFourApp()
    return app.run(sys.argv)

if __name__ == "__main__":
    sys.exit(main())
