import React from "react";
import { render, screen } from "@testing-library/react";
import { describe, it, expect } from "vitest";
import { SemsemCard } from "../SemsemCard";

describe("SemsemCard", () => {
  it("shows ذكي badge for pro", () => {
    render(<SemsemCard s={{ uid_hex: "aa", title: "x", is_pro: true, role: "doctor" }} />);
    expect(screen.getByText("ذكي")).toBeInTheDocument();
  });
  it("shows عادي badge for regular", () => {
    render(<SemsemCard s={{ uid_hex: "aa", title: "x", is_pro: false, role: "" }} />);
    expect(screen.getByText("عادي")).toBeInTheDocument();
  });
});
