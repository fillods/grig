#ifndef __GRIG_KEYPAD_H__
#define __GRIG_KEYPAD_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtktable.h>


G_BEGIN_DECLS

#define GRIG_KEYPAD_TYPE            (grig_keypad_get_type ())
#define GRIG_KEYPAD(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRIG_KEYPAD_TYPE, GrigKeypad))
#define GRIG_KEYPAD_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GRIG_KEYPAD_TYPE, GrigKeypadClass))
#define IS_GRIG_KEYPAD(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GRIG_KEYPAD_TYPE))
#define IS_GRIG_KEYPAD_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GRIG_KEYPAD_TYPE))


typedef struct _GrigKeypad       GrigKeypad;
typedef struct _GrigKeypadClass  GrigKeypadClass;

struct _GrigKeypad
{
	GtkTable table;

	GtkWidget *buttons[10];
	GtkWidget *enter;
	GtkWidget *clear;

	gboolean enabled;
};

struct _GrigKeypadClass
{
	GtkTableClass parent_class;

	void (* grig_keypad) (GrigKeypad *obj);
};

GType          grig_keypad_get_type        (void);
GtkWidget*     grig_keypad_new             (void);
void	       grig_keypad_disable(GrigKeypad *obj);
void	       grig_keypad_enable(GrigKeypad *obj);

G_END_DECLS

#endif /* __GRIG_KEYPAD_H__ */
