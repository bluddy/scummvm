# ScummVM Touch GUI Analysis

This document analyzes the current state of the ScummVM graphical user interface (GUI) on touch-based platforms, specifically Android and iOS. The goal is to identify areas for improvement to enhance the user experience on these devices.

## 1. Touch Control Modes

ScummVM offers several touch control modes to accommodate different user preferences and game types. These modes are configurable in the backend settings.

### 1.1. Android

Android users have three primary touch modes:

*   **Direct Mouse:** This is the most straightforward mode. The pointer directly follows the user's finger on the screen. This mode is the default for menus.
*   **Touchpad Emulation:** This mode simulates a laptop touchpad. The user can move the pointer without their finger being directly over it. This is the default for 2D games.
*   **Gamepad Emulation:** This mode is designed for games that are better suited to a gamepad. It overlays virtual directional pads and action buttons on the screen. This is the default for 3D games.

Users can switch between these modes at any time by tapping a controller icon in the upper right corner of the screen.

### 1.2. iOS

iOS users have two primary touch modes:

*   **Direct Mouse:** Similar to the Android implementation, the pointer follows the user's finger.
*   **Touchpad Emulation:** Also similar to Android, this mode allows for indirect pointer control.

The default modes for menus, 2D games, and 3D games are the same as on Android. iOS also provides a controller icon for switching between modes.

## 2. On-Screen Controls

Both platforms provide on-screen controls to facilitate interaction with the game and the ScummVM interface.

### 2.1. Android

*   **Controller Icon:** Toggles between the different touch control modes.
*   **Menu Icon:** Opens the Global Main Menu.
*   **Virtual Keyboard:** Can be opened by a long press on the controller icon or by tapping an editable text field.

### 2.2. iOS

*   **Controller Icon:** Toggles between Direct Mouse and Touchpad Emulation modes.
*   **Menu Icon:** Opens the Global Main Menu.
*   **Virtual Keyboard:** Can be opened with a pinch gesture or by tapping an editable text field.

## 3. Gestures

Both platforms support a variety of touch gestures to perform common actions.

### 3.1. Android

*   **One finger tap:** Left mouse click.
*   **One finger tap and hold (>0.5s):** Right mouse click.
*   **One finger tap and hold (>1s):** Middle mouse button click.
*   **Double tap + movement:** Left mouse button hold and drag.
*   **Two finger tap:** Right mouse click.
*   **Two finger tap + movement of second finger:** Right mouse button hold and drag.
*   **Two finger slide up/down:** Scroll (reverse mouse wheel).
*   **Three finger tap:** Middle mouse button click.
*   **Three finger tap + movement of third finger:** Middle mouse button hold and drag.

### 3.2. iOS

*   **One finger tap:** Left mouse click.
*   **Two finger tap:** Right mouse click.
*   **Two finger double tap:** ESC.
*   **One finger press & hold (>0.5s):** Left mouse button hold and drag.
*   **Two fingers press & hold (>0.5s):** Right mouse button hold and drag.
*   **Two finger swipe (left to right):** Toggles between touch direct mode and touchpad mode.
*   **Two finger swipe (right to left):** Show/hide on-screen control (iOS 15 and later).
*   **Two finger swipe (top to bottom):** Global Main Menu.
*   **Three finger swipe:** Arrow keys.
*   **Pinch gesture:** Enables/disables keyboard.

## 4. Analysis and Recommendations

The current touch GUI provides a good foundation, but there are several areas where it could be improved.

### 4.1. Inconsistent Gestures

There are significant inconsistencies in the gestures between Android and iOS. For example, a two-finger tap is a right-click on Android, but on iOS, it's also a right-click, but a two-finger *double tap* is ESC. This can be confusing for users who switch between devices.

**Recommendation:** Unify the gesture set across both platforms as much as possible. Where platform-specific gestures are necessary, they should be clearly documented and intuitive.

### 4.2. On-Screen Controls

The on-screen controls are minimal, which is good for not obscuring the game view. However, they could be more discoverable and provide more functionality.

**Recommendation:**

*   **Consolidated Menu:** Instead of separate icons for the menu and controller, consider a single, consolidated menu that provides access to all settings, including touch controls. This would declutter the interface and make it easier for users to find what they're looking for.
*   **Customizable Controls:** Allow users to customize the on-screen controls. This could include the ability to add, remove, and reposition buttons for common actions. This would be particularly useful for the Gamepad Emulation mode.
*   **Haptic Feedback:** Provide haptic feedback for on-screen button presses to give users a more tactile experience.

### 4.3. Touchpad Emulation

The touchpad emulation mode is a good idea, but the implementation could be improved. The pointer speed can be adjusted, but the acceleration and sensitivity could be more refined to provide a smoother and more precise experience.

**Recommendation:**

*   **Improved Pointer Ballistics:** Implement more sophisticated pointer ballistics to make the touchpad emulation feel more natural and responsive.
*   **Scrolling:** The two-finger slide for scrolling is a good start, but it could be more consistent and reliable. Consider adding a dedicated scroll bar or other visual cues to make scrolling easier.

### 4.4. Game-Specific Configurations

While there are different touch modes for 2D and 3D games, there is no way to save game-specific touch configurations. This means that users have to manually change the settings every time they switch between games with different control schemes.

**Recommendation:**

*   **Per-Game Profiles:** Allow users to create and save per-game touch control profiles. This would make it much easier to switch between games without having to reconfigure the controls every time.

## 5. Default GUI Analysis

The default ScummVM GUI, while functional with a mouse and keyboard, is not well-suited for touch-based devices. The core of the issue lies in the design of the widgets and the overall layout, which are inherited from a desktop-first approach.

### 5.1. Widget Design

Many of the core GUI widgets are difficult to use on a touch screen:

*   **Buttons (`ButtonWidget`):** The buttons are often small and clustered together, making them difficult to tap accurately with a finger.
*   **Scrollbars (`ScrollbarWidget`):** The scrollbars are thin and require precise interaction to drag, which is challenging on a touch screen. There is no support for direct list scrolling with a finger.
*   **Pop-up Menus (`PopUpWidget`):** These menus appear as traditional dropdowns, with small text and tight spacing, making them difficult to navigate.
*   **Checkboxes and Radio Buttons (`CheckboxWidget`, `RadiobuttonWidget`):** These are also small and can be difficult to tap accurately.

### 5.2. Theme and Layout

The existing themes, including "scummmodern", improve the visual appearance of the GUI, but they do not fundamentally change the layout or the size of the widgets. The layout is static and does not adapt well to different screen sizes and orientations, which is a common characteristic of mobile devices.

### 5.3. Recommendations

To create a truly touch-friendly experience, the default GUI needs a significant overhaul. Here are some recommendations:

*   **Larger Touch Targets:** All interactive elements, including buttons, checkboxes, and list items, should have a minimum touch target size of 48x48dp, as recommended by the Android and iOS design guidelines.
*   **Touch-Friendly Widgets:**
    *   **Scrollbars:** Replace traditional scrollbars with the ability to scroll lists and text views by directly dragging the content with a finger. Scroll indicators can be made larger and more prominent during scrolling.
    *   **Pop-up Menus:** Redesign pop-up menus to be full-screen or to occupy a significant portion of the screen, with large, easily tappable items.
*   **Responsive Layout:** The GUI should use a responsive layout that adapts to different screen sizes and orientations. This could involve using a more flexible grid system or providing different layouts for portrait and landscape modes.
*   **A Dedicated Touch Theme:** Create a new, modern theme specifically for touch devices. This theme should feature:
    *   Larger fonts and more generous spacing.
    *   A clean, uncluttered design that prioritizes content.
    *   Modern iconography and visual elements that are consistent with the Android and iOS design languages.

## 6. Conclusion

The ScummVM touch GUI is functional, but it could be significantly improved with a few key changes. By unifying gestures, improving the on-screen controls, refining the touchpad emulation, and adding per-game configurations, we can create a much more enjoyable and user-friendly experience for touch-based players. Furthermore, a comprehensive redesign of the default GUI, with a focus on larger touch targets, touch-friendly widgets, and a responsive layout, is essential for making ScummVM a truly first-class experience on mobile devices.
