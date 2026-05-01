import { test, expect } from "@playwright/test";
import { mockBackend } from "./helpers";

test.describe("Devices", () => {
  test("shows device cards and add button", async ({ page }) => {
    await mockBackend(page, {
      "/api/auth/me": { id: 1, email: "a@b.c" },
      "/api/devices": [
        { device_id: "aabbccddeeff", name: "جهاز نديم", online: true, last_seen_at: null, firmware_version: "0.1", today_listening_ms: 600000, total_listening_ms: 3600000, total_semsems: 3 },
      ],
    });

    await page.goto("/devices");
    await expect(page.getByText("الأجهزة")).toBeVisible();
    await expect(page.getByText("جهاز نديم")).toBeVisible();
    await expect(page.getByText("نشط خلال آخر 5 دقائق")).toBeVisible();
    await expect(page.getByText("دقائق اليوم")).toBeVisible();
    await expect(page.getByRole("button", { name: "add إضافة جهاز", exact: true })).toBeVisible();
  });

  test("shows offline recent-activity copy", async ({ page }) => {
    await mockBackend(page, {
      "/api/auth/me": { id: 1, email: "a@b.c" },
      "/api/devices": [
        { device_id: "aabbccddeeff", name: "جهاز نديم", online: false, last_seen_at: "2026-05-01T10:00:00Z", firmware_version: "0.1", today_listening_ms: 0, total_listening_ms: 0, total_semsems: 0 },
      ],
    });

    await page.goto("/devices");
    await expect(page.getByText("غير نشط حالياً")).toBeVisible();
  });
});
