# Instructions

- Following Playwright test failed.
- Explain why, be concise, respect Playwright best practices.
- Provide a snippet of code with the fix, if possible.

# Test info

- Name: login.spec.ts >> redirects unauth user to /login
- Location: e2e/login.spec.ts:3:5

# Error details

```
Error: page.goto: net::ERR_CONNECTION_REFUSED at http://localhost:3000/
Call log:
  - navigating to "http://localhost:3000/", waiting until "load"

```

# Test source

```ts
  1  | import { test, expect } from "@playwright/test";
  2  | 
  3  | test("redirects unauth user to /login", async ({ page }) => {
> 4  |   await page.goto("/");
     |              ^ Error: page.goto: net::ERR_CONNECTION_REFUSED at http://localhost:3000/
  5  |   await expect(page).toHaveURL(/\/login$/);
  6  | });
  7  | 
  8  | test("login with valid creds → dashboard", async ({ page }) => {
  9  |   await page.goto("/login");
  10 |   await page.getByLabel("البريد الإلكتروني").fill("parent@example.com");
  11 |   await page.getByLabel("كلمة المرور").fill("hunter2");
  12 |   await page.getByRole("button", { name: "تسجيل الدخول" }).click();
  13 |   await expect(page).toHaveURL("/");
  14 | });
  15 | 
```