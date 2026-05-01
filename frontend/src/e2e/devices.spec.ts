import { test, expect } from "@playwright/test";

test.describe("Devices", () => {
  test("shows device cards and add button", async ({ page }) => {
    await page.route("**/api/auth/me", (r) => r.fulfill({ status: 200, body: JSON.stringify({ id: 1, email: "a@b.c" }), contentType: "application/json" }));
    await page.route("**/api/devices", (r) => r.fulfill({ status: 200, body: JSON.stringify([
      { device_id: "aabbccddeeff", name: "جهاز نديم", online: true, last_seen_at: null, firmware_version: "0.1", total_listening_ms: 3600000, total_semsems: 3 },
    ]), contentType: "application/json" }));

    await page.goto("/devices");
    await expect(page.getByText("الأجهزة")).toBeVisible();
    await expect(page.getByText("جهاز نديم")).toBeVisible();
    await expect(page.getByText("متصل الآن")).toBeVisible();
    await expect(page.getByText("إضافة جهاز")).toBeVisible();
  });
});
