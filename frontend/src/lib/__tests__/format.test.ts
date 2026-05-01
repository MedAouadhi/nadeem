import { describe, it, expect } from "vitest";
import { msToHours, msToMinutes, fmtRelative } from "../format";

describe("format", () => {
  it("converts ms to hours", () => { expect(typeof msToHours(3_600_000)).toBe("string"); });
  it("rounds minutes", () => { expect(msToMinutes(120_000)).toMatch(/2/); });
  it("shows today for recent dates", () => { expect(fmtRelative(new Date().toISOString())).toBe("اليوم"); });
});
