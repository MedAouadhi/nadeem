import { test, expect } from "@playwright/test";

test("redirects unauth user to /login", async ({ page }) => {
  await page.goto("/");
  await expect(page).toHaveURL(/\/login$/);
});

test("login with valid creds → dashboard", async ({ page }) => {
  await page.goto("/login");
  await page.getByLabel("البريد الإلكتروني").fill("parent@example.com");
  await page.getByLabel("كلمة المرور").fill("hunter2");
  await page.getByRole("button", { name: "تسجيل الدخول" }).click();
  await expect(page).toHaveURL("/");
});

test("signup with new email → dashboard", async ({ page }) => {
  const email = `signup-test-${Date.now()}@example.com`;
  await page.goto("/signup");
  await page.getByLabel("البريد الإلكتروني").fill(email);
  await page.getByLabel("كلمة المرور").fill("testpass123");
  await page.getByRole("button", { name: "إنشاء حساب" }).click();
  await expect(page).toHaveURL("/");
});

test("logo is visible on login page", async ({ page }) => {
  await page.goto("/login");
  const logo = page.locator('img[alt="نديم"]');
  await expect(logo).toBeVisible();
});
