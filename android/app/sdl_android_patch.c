/* SDL2 2.32.x Android joystick forward declarations.
 * These are in SDL2's src/joystick/android/SDL_sysjoystick_c.h
 * but not included by SDL_android.c at compile time. */
void Android_OnPadDown(int device_id, int keycode);
void Android_OnPadUp(int device_id, int keycode);
void Android_OnJoy(int device_id, int axis, float value);
void Android_OnHat(int device_id, int hat, int value);
int Android_AddJoystick(int device_id, const char *name, const char *desc,
                        int vendor, int product, int button_mask, int naxes,
                        int axis_mask, int nhats, int nballs);
int Android_RemoveJoystick(int device_id);
int Android_AddHaptic(int device_id, const char *name, int vendor, int product);
int Android_RemoveHaptic(int device_id);