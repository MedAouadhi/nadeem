# Web Design System

> **Type:** UI / Style
> **Status:** spec-only; tokens lifted from `frontend/design.md` mockup

## Layout

- **Direction:** RTL strict, `<html dir="rtl" lang="ar">`.
- **Container:** Tailwind utility classes; max-width per screen (login `max-w-md`, dashboard wider).

## Typography

- **Headline:** `'Baloo Bhaijaan 2'`, weights 400/700/800.
- **Body / labels:** `'Cairo'` (or `'Tajawal'`), weights 400/600/700.
- Material Symbols Outlined for icons.

## Color tokens (Material 3 style)

From `frontend/design.md` Tailwind config:

| Token | Value |
|-------|-------|
| primary | `#00668a` |
| primary-container | `#4fb6e9` |
| on-primary | `#ffffff` |
| secondary | `#875200` |
| secondary-container | `#fdaa3f` |
| tertiary | `#686000` |
| tertiary-container | `#baae3b` |
| surface | `#fafaf5` |
| surface-container-lowest | `#ffffff` |
| surface-container-low | `#f4f4ef` |
| surface-container | `#eeeee9` |
| surface-container-high | `#e8e8e3` |
| surface-container-highest | `#e3e3de` |
| on-background / on-surface | `#1a1c19` |
| on-surface-variant | `#3e484f` |
| outline | `#6f7880` |
| outline-variant | `#bec8d0` |
| error | `#ba1a1a` |
| error-container | `#ffdad6` |
| inverse-primary | `#7cd0ff` |

## Border radii

| Token | Value |
|-------|-------|
| DEFAULT | `1rem` |
| lg | `2rem` |
| xl | `3rem` |
| full | `9999px` |

## Patterns

- Login card: `bg-surface-container-highest`, soft shadow `shadow-[0_8px_30px_rgb(0,0,0,0.04)]`, blurred decorative blobs in primary/secondary container colors.
- Buttons: `rounded-full` gradient `from-primary to-primary-container`, `font-headline font-bold`, hover scale `1.02`.
- Inputs: rounded `md`, ring-inset outline, focus → `ring-primary`.

## Badging

- "عادي" (Regular): neutral / surface.
- "ذكي" (Pro): primary or tertiary container highlight.

---
confidence: 0.85
sources: [S9, S7]
last_confirmed: 2026-04-25
status: spec-only
