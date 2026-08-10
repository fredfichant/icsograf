# User Manual Design

Date: 2026-08-10
Project: icsograf
Scope: Add a locally bundled user manual opened from the existing Help menu command.

## Goal

Provide an offline user manual that ships with the application and can be opened from the existing Help menu entry. The manual should help end users understand the main workflows of the application without requiring internet access.

## Problem Statement

The Help menu already exposes a manual command through `Main_Window::handleManualTriggered()`, but it currently opens `DOC_URL`. That is suitable for a remote document, but not for a manual guaranteed to be available offline. We need a bundled manual with predictable availability in the build tree and in installed packages.

## User Outcome

When a user clicks the manual entry in Help:

- the application opens a local HTML manual,
- the manual works without network access,
- the content is readable in a browser,
- if the manual file is missing, the user gets a clear error message instead of a broken URL.

## Recommended Approach

Bundle a static HTML manual under application data and open it through a local file URL resolved at runtime.

This is the best fit because:

- it matches the current `QDesktopServices::openUrl()` integration,
- it avoids adding a custom in-app documentation viewer,
- it works offline,
- it keeps the manual easy to edit and expand.

## Alternatives Considered

### 1. Local HTML manual opened with `QDesktopServices`

Recommended.

Pros:

- minimal code changes,
- good reading experience,
- easy to style and structure,
- works offline.

Cons:

- content is shown outside the Qt window,
- depends on the system browser association.

### 2. Local HTML manual inside a dedicated Qt dialog

Not recommended for this scope.

Pros:

- more integrated user experience.

Cons:

- adds UI code and maintenance burden,
- requires an HTML rendering strategy inside the application,
- more risk than needed for a first manual.

### 3. Markdown or plain text opened directly

Not recommended.

Pros:

- easy to author.

Cons:

- weaker user experience,
- formatting is less predictable across systems,
- less aligned with the current Help action.

## Content Location

The manual will live in:

- `data/help/manual.html`

Rationale:

- `data/` is already copied into the build tree,
- `data/` is already installed with the application,
- this keeps user-facing runtime documentation separate from developer-oriented `docs/`.

## Manual Content

The initial manual should focus on the core workflows an end user needs.

Sections:

1. What icsograf is
2. Getting started
3. Creating a new graph
4. Adding and editing nodes and edges
5. Navigating the canvas
6. Saving and reopening work
7. Exporting SVG or images
8. Using the graph library/browser
9. Tips, limitations, and troubleshooting

Non-goals for the first version:

- exhaustive reference for every menu action,
- animated help,
- screenshots pipeline,
- localization system,
- searchable in-app documentation.

## Runtime Integration

The Help menu action is already implemented in:

- `src/dialogs/main_window.cpp`

Current behavior:

- `handleManualTriggered()` opens `DOC_URL`.

Target behavior:

- `handleManualTriggered()` resolves the bundled manual path from application data,
- converts that path to a local file URL,
- opens it with `QDesktopServices::openUrl()`,
- falls back to a user-visible error dialog if the file cannot be found.

## Data Resolution Strategy

The implementation should reuse the same runtime data lookup conventions already used elsewhere in the app, rather than hardcoding development-only paths.

Expected resolution order:

1. bundled runtime data in the build tree,
2. installed application data location,
3. explicit failure with a clear message if neither is found.

This keeps behavior correct both during development and after installation.

## HTML Requirements

The manual HTML should be self-contained:

- no external CSS,
- no external JavaScript,
- no remote fonts,
- no remote images for the first version.

The page should include:

- a page title,
- a visible heading,
- a simple table of contents with anchor links,
- readable typography,
- narrow line length,
- stable layout on common desktop widths.

## Error Handling

If the local manual cannot be found or opened:

- show a `QMessageBox::warning`,
- explain that the local manual is unavailable,
- include the attempted path when useful for diagnosis.

We should not silently fail and should not fall back to an unrelated web URL in the offline-manual design.

## Files Expected To Change

- `data/help/manual.html`
- `src/dialogs/main_window.cpp`
- possibly a helper source already responsible for application data lookup if needed
- build or install configuration only if the current `data/` handling does not already cover `data/help/`

## Testing Strategy

Manual verification:

1. build the application,
2. run `./build/bin/icsograf`,
3. trigger Help -> Manual,
4. verify the browser opens the local bundled HTML manual,
5. verify the manual remains accessible from the build tree,
6. verify the content includes the expected core sections.

Failure-path verification:

1. temporarily make the manual unavailable,
2. trigger Help -> Manual,
3. verify the application shows a clear warning dialog.

Regression scope:

- the existing Help menu action still works,
- packaging and build data copying still include the manual,
- no network requirement is introduced.

## Risks

### Runtime path mismatch

The biggest risk is resolving the local manual path differently in the build tree versus an installed application.

Mitigation:

- reuse the application’s existing data path conventions,
- test both build-tree behavior and expected installed-path behavior conceptually in code review.

### Manual drift

The manual may become outdated as features evolve.

Mitigation:

- keep the first version focused on stable workflows,
- store it in a clearly maintained runtime data path,
- update it alongside visible workflow changes.

## Implementation Boundaries

This work should not:

- redesign the Help menu,
- add a new documentation viewer widget,
- introduce a documentation generation pipeline,
- add remote dependencies,
- change unrelated About or bug-report actions.

## Success Criteria

This feature is complete when:

- a bundled local HTML manual exists,
- Help -> Manual opens that local file,
- the manual is readable and useful for first-time users,
- missing-manual failure is handled cleanly,
- the build still succeeds and affected behavior is manually verified.
