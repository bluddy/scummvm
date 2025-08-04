# ScummVM Touch GUI Implementation Plan

This document outlines a step-by-step plan for creating a new, touch-friendly theme for the ScummVM GUI. This plan is based on the analysis of the existing GUI structure and the needs of touch-based devices.

## 1. Project Goals

*   Create a new GUI theme, tentatively named "TouchModern", specifically designed for touch-based devices.
*   Ensure all interactive elements are large enough to be easily tapped with a finger.
*   Implement a responsive layout that adapts to different screen sizes and orientations.
*   Improve the usability of core GUI widgets, such as scrollbars and pop-up menus.
*   Maintain a clean and modern aesthetic that is consistent with the design languages of Android and iOS.

## 2. High-Level Plan

The project will be divided into the following phases:

1.  **Theme Scaffolding:** Create the basic file structure and configuration for the new theme.
2.  **Widget Redesign:** Redesign and implement touch-friendly versions of the core GUI widgets.
3.  **Layout Implementation:** Create a responsive layout that adapts to different screen sizes and orientations.
4.  **Refinement and Testing:** Thoroughly test the new theme on a variety of devices and make any necessary adjustments.

## 3. Detailed Plan

### Phase 1: Theme Scaffolding

1.  **Create a new theme directory:** Create a new directory named `touchmodern` inside the `gui/themes` directory.
2.  **Create a theme file:** Inside the `touchmodern` directory, create a new file named `touchmodern.theme`. This file will contain the XML-based theme definition.
3.  **Define the color palette:** In `touchmodern.theme`, define a new color palette that is clean, modern, and has good contrast.
4.  **Define the fonts:** In `touchmodern.theme`, define a new set of fonts that are easy to read on a variety of screen sizes. Use a modern, sans-serif font like Material Symbols Sharp, which is already included in the codebase.
5.  **Update the build system:** Add the new theme to the `Makefile` in the `gui/themes` directory to ensure it is included in the build.

### Phase 2: Widget Redesign

This phase will focus on redesigning the core GUI widgets to be more touch-friendly. The following widgets will be addressed:

*   **ButtonWidget:**
    *   Increase the default size of buttons to at least 48x48dp.
    *   Add more padding around the button text.
    *   Implement visual feedback for touch events, such as a ripple effect or a change in elevation.
*   **ScrollbarWidget:**
    *   Replace the traditional scrollbar with a larger, more easily draggable scroll indicator.
    *   Implement the ability to scroll lists and text views by directly dragging the content with a finger.
*   **PopUpWidget:**
    *   Redesign pop-up menus to be full-screen or to occupy a significant portion of the screen.
    *   Use large, easily tappable list items.
*   **CheckboxWidget and RadiobuttonWidget:**
    *   Increase the size of the checkbox and radio button icons.
    *   Increase the touch target area around the widgets.

### Phase 3: Layout and Navigation Implementation

This phase will focus on creating a responsive layout and implementing a modern, touch-friendly navigation system.

1.  **Create a new layout file:** Inside the `touchmodern` directory, create a new file named `touchmodern_layout.stx`.
2.  **Implement a Hamburger Menu:**
    *   Replace the separate on-screen icons for the menu and controller with a single hamburger menu icon.
    *   The hamburger menu will provide access to all global actions, including the main menu, touch control settings, and the virtual keyboard.
3.  **Implement a Tab-Based Configuration Manager:**
    *   Redesign the options dialog to be a full-screen, multi-page configuration manager, similar to the native Android or iOS settings apps.
    *   Use a tab-based navigation system to switch between different categories of settings (e.g., Graphics, Audio, Controls).
    *   Each category will have its own page with a clear and consistent layout.
    *   Use sliding toggles instead of small checkboxes for boolean options.
4.  **Define a responsive grid system:** Use a flexible grid system that allows the layout to adapt to different screen widths.
5.  **Create different layouts for portrait and landscape modes:** Provide different layouts for portrait and landscape orientations to make the best use of the available screen space.
6.  **Implement a more modern and touch-friendly design for the main launcher and other key dialogs.**

### Phase 4: Refinement and Testing

This phase will focus on testing the new theme on a variety of devices and making any necessary adjustments.

1.  **Test on a variety of Android and iOS devices:** Test the new theme on a range of devices with different screen sizes and resolutions.
2.  **Gather feedback from users:** Make the new theme available to a group of beta testers and gather their feedback.
3.  **Iterate on the design:** Based on the feedback from testing, make any necessary adjustments to the theme and layout.

## 4. Timeline

This project is expected to take approximately 4-6 weeks to complete. The following is a rough timeline:

*   **Week 1:** Theme Scaffolding
*   **Weeks 2-3:** Widget Redesign
*   **Weeks 4-5:** Layout Implementation
*   **Week 6:** Refinement and Testing

## 5. Conclusion

By following this plan, we can create a new, touch-friendly theme for the ScummVM GUI that will significantly improve the user experience on Android and iOS devices. This will make ScummVM more accessible and enjoyable for a wider range of users.
