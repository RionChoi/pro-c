import { describe, it, expect } from "vitest";
import { hasRole } from "../src/rbac";

describe("rbac — hasRole", () => {
  describe("null/undefined user role", () => {
    it("should deny access for null role", () => {
      expect(hasRole(null, "ADMIN")).toBe(false);
    });

    it("should deny access for undefined role", () => {
      expect(hasRole(undefined, "MEMBER")).toBe(false);
    });

    it("should deny access for empty string role", () => {
      expect(hasRole("", "ADMIN")).toBe(false);
    });
  });

  describe("ADMIN super-user", () => {
    it("should always pass regardless of required roles", () => {
      expect(hasRole("ADMIN", "ADMIN")).toBe(true);
      expect(hasRole("ADMIN", "MEMBER")).toBe(true);
      expect(hasRole("ADMIN", "PARTNER")).toBe(true);
      expect(hasRole("ADMIN", "VIEWER")).toBe(true);
    });

    it("should pass even with no required roles (login-only check)", () => {
      expect(hasRole("ADMIN")).toBe(true);
    });
  });

  describe("regular roles", () => {
    it("MEMBER should access MEMBER-required resources", () => {
      expect(hasRole("MEMBER", "MEMBER")).toBe(true);
    });

    it("MEMBER should NOT access ADMIN-required resources", () => {
      expect(hasRole("MEMBER", "ADMIN")).toBe(false);
    });

    it("PARTNER should access PARTNER-required resources", () => {
      expect(hasRole("PARTNER", "PARTNER")).toBe(true);
    });

    it("PARTNER should NOT access MEMBER-required resources", () => {
      expect(hasRole("PARTNER", "MEMBER")).toBe(false);
    });

    it("VIEWER should access VIEWER-required resources", () => {
      expect(hasRole("VIEWER", "VIEWER")).toBe(true);
    });
  });

  describe("multiple allowed roles", () => {
    it("should pass if user has any of the required roles", () => {
      expect(hasRole("MEMBER", "MEMBER", "PARTNER")).toBe(true);
      expect(hasRole("PARTNER", "MEMBER", "PARTNER")).toBe(true);
    });

    it("should fail if user has none of the required roles", () => {
      expect(hasRole("VIEWER", "MEMBER", "PARTNER")).toBe(false);
    });
  });

  describe("no required roles (login-only)", () => {
    it("should pass for any valid role when no roles required", () => {
      expect(hasRole("MEMBER")).toBe(true);
      expect(hasRole("PARTNER")).toBe(true);
      expect(hasRole("VIEWER")).toBe(true);
    });
  });
});
