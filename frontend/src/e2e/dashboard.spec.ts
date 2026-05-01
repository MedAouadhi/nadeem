import { test, expect } from "@playwright/test";
import { mockBackend } from "./helpers";

test.describe("Dashboard", () => {
  test("shows dashboard heading for authenticated user", async ({ page }) => {
    await mockBackend(page, {
      "/api/auth/me": { id: 1, email: "a@b.c" },
      "/api/users/me/stats": { today_listening_ms: 3600000, total_listening_ms: 5400000, today_pro_ms: 600000, total_pro_ms: 1200000, unique_semsems: 5, device_count: 1, online_device_count: 1 },
      "/api/devices": [{ device_id: "aabbccddeeff", name: "جهاز نديم", online: true, last_seen_at: null, today_listening_ms: 600000, total_listening_ms: 5400000, total_semsems: 3 }],
    });

    await page.goto("/");
    await expect(page.getByText("مرحباً، أهالي نديم!")).toBeVisible();
    await expect(page.getByText("إجمالي دقائق الاستماع")).toBeVisible();
    await expect(page.getByRole("heading", { name: "90" })).toBeVisible();
    await expect(page.getByText("نشط خلال آخر 5 دقائق")).toBeVisible();
  });
});
