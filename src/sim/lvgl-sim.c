/* pixel.c
 *
 * Compile: cc -ggdb pixel.c -o pixel $(pkg-config --cflags --libs gtk4) -o pixel
 * Run: ./pixel
 *
 * Author: Mohammed Sadiq <www.sadiqpk.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later OR CC0-1.0
 */

#include <gtk/gtk.h>

#define BYTES_PER_R8G8B8 3
#define WIDTH  400

static void
fill_row (GByteArray *array,
          guint8      value,
          int         row_size)
{
  guint i;

  for (i = 0; i < row_size; i++) {
    /* Fill the same values for RGB */
    g_byte_array_append (array, &value, 1); /* R */
    g_byte_array_append (array, &value, 1); /* G */
    g_byte_array_append (array, &value, 1); /* B */
  }
}

static void
add_pixel_picture (GtkPicture *picture)
{
  g_autoptr(GBytes) bytes = NULL;
  GdkTexture *texture;
  GByteArray *pixels;
  gsize height;

  /* Draw something interesting */
  pixels = g_byte_array_new ();
  for (guint i = 0; i <= 0xff ; i++)
    fill_row (pixels, i, WIDTH);

  for (guint i = 0; i <= 0xff; i++)
    fill_row (pixels, 0xff - i, WIDTH);

  height = pixels->len / (WIDTH * BYTES_PER_R8G8B8);
  bytes = g_byte_array_free_to_bytes (pixels);

  texture = gdk_memory_texture_new (WIDTH, height,
                                    GDK_MEMORY_R8G8B8,
                                    bytes,
                                    WIDTH * BYTES_PER_R8G8B8);
  gtk_picture_set_paintable (picture, GDK_PAINTABLE (texture));
}

static void
app_activated_cb (GtkApplication *app)
{
  GtkWindow *window;
  GtkWidget *picture;

  window = GTK_WINDOW (gtk_application_window_new (app));
  g_object_set (window,
                "width-request", 500,
                "height-request", 400,
                NULL);
  picture = gtk_picture_new ();
  gtk_widget_add_css_class (picture, "frame");
  g_object_set (picture,
                "margin-start", 96,
                "margin-end", 96,
                "margin-top", 96,
                "margin-bottom", 96,
                NULL);

  gtk_window_set_child (window, picture);
  add_pixel_picture (GTK_PICTURE (picture));

  gtk_window_present (window);
}

int
main (int   argc,
      char *argv[])
{
  g_autoptr(GtkApplication) app = gtk_application_new (NULL, 0);

  g_signal_connect (app, "activate", G_CALLBACK (app_activated_cb), NULL);

  return g_application_run (G_APPLICATION (app), argc, argv);
}

