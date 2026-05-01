import type { Page } from "@playwright/test";

const BASE_URL = "http://localhost:3001";

export async function mockBackend(page: Page, mocks: Record<string, unknown>) {
  await page.context().addCookies([
    { name: "nadeem_jwt", value: "test-jwt", url: BASE_URL },
    { name: "nadeem_e2e_backend_mocks", value: encodeURIComponent(JSON.stringify(mocks)), url: BASE_URL },
  ]);
}
