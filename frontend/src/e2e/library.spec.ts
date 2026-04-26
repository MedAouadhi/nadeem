import { test, expect } from "@playwright/test";

test.describe("Library", () => {
  test("shows semsem cards", async ({ page }) => {
    await page.route("**/api/auth/me", (r) => r.fulfill({ status: 200, body: JSON.stringify({ id: 1, email: "a@b.c" }), contentType: "application/json" }));
    await page.route("**/api/semsems", (r) => r.fulfill({ status: 200, body: JSON.stringify([
      { uid_hex: "aa", title: "دكتور سعيد", is_pro: true, role: "doctor", description: "طبيب ذكي" },
      { uid_hex: "bb", title: "قصة الأرنب", is_pro: false, role: "", description: "قصة عادية" },
    ]), contentType: "application/json" }));

    await page.goto("/library");
    await expect(page.getByText("مكتبة السماسم")).toBeVisible();
    await expect(page.getByText("دكتور سعيد")).toBeVisible();
    await expect(page.getByText("ذكي")).toBeVisible();
    await expect(page.getByText("قصة الأرنب")).toBeVisible();
    await expect(page.getByText("عادي")).toBeVisible();
  });
});
