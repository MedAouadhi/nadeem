import { test, expect } from "@playwright/test";

test.describe("Dashboard", () => {
  test("shows dashboard heading for authenticated user", async ({ page }) => {
    await page.route("**/api/auth/me", (r) => r.fulfill({ status: 200, body: JSON.stringify({ id: 1, email: "a@b.c" }), contentType: "application/json" }));
    await page.route("**/api/users/me/stats", (r) => r.fulfill({ status: 200, body: JSON.stringify({ total_listening_ms: 3600000, unique_semsems: 5, pro_total_ms: 0, device_count: 1, online_device_count: 1 }), contentType: "application/json" }));
    await page.route("**/api/devices", (r) => r.fulfill({ status: 200, body: JSON.stringify([{ device_id: "aabbccddeeff", name: "جهاز نديم", online: true, last_seen_at: null }]), contentType: "application/json" }));

    await page.goto("/");
    await expect(page.getByText("مرحباً، أهالي نديم!")).toBeVisible();
    await expect(page.getByText("1")).toBeVisible();
  });
});
