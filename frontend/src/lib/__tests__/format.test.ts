import { describe, it, expect } from "vitest";
import { msToMinutes, fmtRelative, presenceCopy } from "../format";

describe("format", () => {
  it("rounds minutes", () => { expect(msToMinutes(120_000)).toMatch(/2/); });
  it("shows recent-activity online copy", () => { expect(presenceCopy(true)).toBe("نشط خلال آخر 5 دقائق"); });
  it("shows recent-activity offline copy", () => { expect(presenceCopy(false)).toBe("غير نشط حالياً"); });
  it("shows today for recent dates", () => { expect(fmtRelative(new Date().toISOString())).toBe("اليوم"); });
});
