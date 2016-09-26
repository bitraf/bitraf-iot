import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk, Gdk
from gi.repository.GdkPixbuf import Pixbuf, InterpType
from gi.repository import GLib
import json
import urllib2
import math

class User(Gtk.ListBoxRow):
    def __init__(self, user):
        super(Gtk.ListBoxRow, self).__init__()
        self.name = user["name"]
        self.login = user["date"]
        label = Gtk.Label("<span font_desc='50px'>"+self.name+"</span>")
        label.set_margin_top(30)
        label.set_margin_bottom(30)
        label.set_use_markup(True)
        self.add(label)

class Karma(Gtk.ListBoxRow):
    def __init__(self, data):
        super(Gtk.ListBoxRow, self).__init__()
        image = Gtk.Image.new_from_file ("images/"+data["image"])
        pixbuf = image.get_pixbuf()
        p = pixbuf.scale_simple(384, 384, InterpType.NEAREST)
        image.set_from_pixbuf(p)

        box = Gtk.Box(spacing=6, orientation=Gtk.Orientation.VERTICAL)
        box.add(image)
        box.get_style_context().add_class("colorize")
        label = Gtk.Label(data["text"])
        label.set_use_markup(True)
        box.add(label)

        self.add(box)

class Handler:
    def __init__(self, window, builder):
        self.window = window
        self.builder = builder


    def onDeleteWindow(self, *args):
        Gtk.main_quit(*args)

    def onKeyPressed(self, window, key):
        if key.hardware_keycode == 9:
            Gtk.main_quit(None)
        elif key.hardware_keycode == 41:
            self.window.unfullscreen()
        #else:
        #    print(key.hardware_keycode)

    def add_users(self, listbox):
        response = urllib2.urlopen('https://p2k12.bitraf.no/checkins/?interval=2+days&media-type=application%2Fvnd.bitraf.checkins-json')
        html = response.read()
        users = json.loads(html)
        #print users
        for user in users:
            listbox.add(User(user))

    def add_karmas(self, listbox):
        with open("karma.json") as f:
            items = json.loads(f.read())
            for item in items["karmas"]:
                listbox.add(Karma(item))

    def on_add_karma_press(self, button, other):
        self.drawgrid = self.builder.get_object("drawgrid")
        self.drawgrid.set_events(Gdk.EventMask.ALL_EVENTS_MASK)
        self.img =  [[0]*24 for i in range(24)]
        self.pressed = [False, False]

        #3self.drawgrid.override_background_color(Gtk.StateType.NORMAL, Gdk.RGBA(1,.5,.5,.5))
        #self.drawgrid.get_style_context().add_class("colorize")
        self.drawbox = self.builder.get_object("drawbox")
        self.drawbox.show()
        #self.offset = self.drawgrid.get_allocated_width()/2 -200

    def on_draw(self, wid, cr):
        print "On draw"
        cr.set_line_width(16)
        cr.set_source_rgb(0, 0, 0)

        for i in range(24):
            for j in range(24):
                if self.img[i][j]:
                    cr.rectangle(i*16, j*16, 16, 16)
                if self.img[i][j]:
	                cr.fill()

    def print_img(self):
        for i in range(24):
            for j in range(24):
                print str(self.img[i][j]),
            print("")
        print("")

    def on_grid_press(self, w, e):
        print "on_grid_press"+str(e.type)+str(e.button)
        if e.type == Gdk.EventType.BUTTON_PRESS:
            if e.button == 1:
                self.pressed[0] = True
            if e.button == 3:
                self.pressed[1] = True

    def on_grid_release(self, w, e):
        print "on_grid_release"
        if e.type == Gdk.EventType.BUTTON_RELEASE:
            if e.button == 1:
                self.pressed[0] = False
            if e.button == 3:
                self.pressed[1] = False
        return 0

    def mouse_move(self, w, e):
        print "Mouse move"
        if self.pressed[0]:
            x = int(e.x/16)
            y = int(e.y/16)
            #print x
            #print y
            if x <16 and y<16:
                self.img[x][y] = 1
            #print ("P {} {}".format(x, y))
            self.drawgrid.queue_draw()
        if self.pressed[1]:
            x = int(e.x/16)
            y = int(e.y/16)
            if x <16 and y<16:
                self.img[x][y] = 0
            #print ("P {} {}".format(x, y))
            #self.drawgrid.draw()
            self.drawgrid.queue_draw()
        return 0

    def on_touch(self, w, e):
        (_, ex, ey) = e.get_coords()
        x = int(ex/16)
        y = int(ey/16)
        print x, y
        self.img[x][y] = 1
        self.drawgrid.draw()
        self.drawgrid.queue_draw()
        return 1
        #self.print_img()

    def on_karma_ok(self, e):
        image = Gtk.Image.new_from_file ("images/karma_0.png")
        pixbuf = image.get_pixbuf()
        #pixbuf.save("/images/karma_99.png", "png")
        p = pixbuf.scale_simple(384, 384, InterpType.NEAREST)
        image.set_from_pixbuf(p)
        img = [item*0xFF for sublist in self.img*4 for item in sublist]
        print img
        data = GLib.Bytes(img)#Pixbuf.GBytes([1, 2, 3, 4], 4)
        print len(data.get_data())
        colorspace = pixbuf.get_colorspace()
        new = Pixbuf.new_from_bytes(data, colorspace, True, 8, 24, 24, 24*4)
        image.set_from_pixbuf(new)

        new.savev("images/karma_99.png", "png", [], [])
        print("New Karma created")
        self.drawbox.hide()

    def on_karma_cancel(self, e):
        self.drawbox.hide()
        print("Karma cancel")

class App:
    def __init__(self):
        builder = Gtk.Builder()
        builder.add_from_file("karma.glade")

        window = builder.get_object("window1")

        h = Handler(window, builder)
        builder.connect_signals(h)

        users = builder.get_object("users")
        h.add_users(users)

        karmas = builder.get_object("karmas")
        h.add_karmas(karmas)

        window.show_all()
        #window.fullscreen()


k = App()
style_provider = Gtk.CssProvider()
css = open('style.css', 'rb')
css_data = css.read()
css.close()
style_provider.load_from_data(css_data)
Gtk.StyleContext.add_provider_for_screen(
    Gdk.Screen.get_default(),
    style_provider,
    Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION)

Gtk.main()
