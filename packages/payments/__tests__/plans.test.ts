import { describe, it, expect } from "vitest";
import { PLAN_CONFIG, getPlanConfig } from "../src/plans";
import type { Plan } from "@repo/db";

describe("plans", () => {
  describe("PLAN_CONFIG", () => {
    it("should have all 4 plans defined", () => {
      const plans: Plan[] = ["FREE", "STARTER", "PRO", "ENTERPRISE"];
      for (const plan of plans) {
        expect(PLAN_CONFIG[plan]).toBeDefined();
      }
    });

    it("FREE plan should have 0 price", () => {
      expect(PLAN_CONFIG.FREE.priceKrw).toBe(0);
    });

    it("paid plans should have positive price", () => {
      expect(PLAN_CONFIG.STARTER.priceKrw).toBeGreaterThan(0);
      expect(PLAN_CONFIG.PRO.priceKrw).toBeGreaterThan(0);
      expect(PLAN_CONFIG.ENTERPRISE.priceKrw).toBeGreaterThan(0);
    });

    it("prices should increase with plan tier", () => {
      expect(PLAN_CONFIG.STARTER.priceKrw).toBeLessThan(PLAN_CONFIG.PRO.priceKrw);
      expect(PLAN_CONFIG.PRO.priceKrw).toBeLessThan(PLAN_CONFIG.ENTERPRISE.priceKrw);
    });

    it("member limits should increase with plan tier", () => {
      expect(PLAN_CONFIG.FREE.maxMembers).toBeLessThan(PLAN_CONFIG.STARTER.maxMembers);
      expect(PLAN_CONFIG.STARTER.maxMembers).toBeLessThan(PLAN_CONFIG.PRO.maxMembers);
      // ENTERPRISE should be unlimited (-1)
      expect(PLAN_CONFIG.ENTERPRISE.maxMembers).toBe(-1);
    });

    it("each plan should have a name", () => {
      for (const plan of Object.values(PLAN_CONFIG)) {
        expect(plan.name).toBeTruthy();
        expect(typeof plan.name).toBe("string");
      }
    });
  });

  describe("getPlanConfig", () => {
    it("should return correct config for each plan", () => {
      expect(getPlanConfig("FREE")).toEqual(PLAN_CONFIG.FREE);
      expect(getPlanConfig("STARTER")).toEqual(PLAN_CONFIG.STARTER);
      expect(getPlanConfig("PRO")).toEqual(PLAN_CONFIG.PRO);
      expect(getPlanConfig("ENTERPRISE")).toEqual(PLAN_CONFIG.ENTERPRISE);
    });
  });
});
