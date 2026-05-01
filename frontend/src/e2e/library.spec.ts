import { test, expect } from "@playwright/test";
import { mockBackend } from "./helpers";

test.describe("Library", () => {
  test("shows semsem cards", async ({ page }) => {
    await mockBackend(page, {
      "/api/auth/me": { id: 1, email: "a@b.c" },
      "/api/semsems": [
        { uid_hex: "aa", title: "دكتور سعيد", is_pro: true, role: "doctor", description: "طبيب ذكي" },
        { uid_hex: "bb", title: "قصة الأرنب", is_pro: false, role: "", description: "قصة عادية" },
      ],
    });

    await page.goto("/library");
    await expect(page.getByRole("heading", { name: "مكتبة السماسم" })).toBeVisible();
    await expect(page.getByText("دكتور سعيد")).toBeVisible();
    await expect(page.getByText("طبيب ذكي")).toBeVisible();
    await expect(page.getByText("قصة الأرنب")).toBeVisible();
    await expect(page.getByRole("button", { name: "عادي" })).toBeVisible();
  });
});
