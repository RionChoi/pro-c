import type { Plan } from "@repo/db";

export interface PlanConfig {
  name: string;
  priceKrw: number;       // 월 요금 (원)
  maxMembers: number;     // -1 = unlimited
  maxProjects: number;    // -1 = unlimited
}

export const PLAN_CONFIG: Record<Plan, PlanConfig> = {
  FREE: {
    name: "무료",
    priceKrw: 0,
    maxMembers: 3,
    maxProjects: 1,
  },
  STARTER: {
    name: "스타터",
    priceKrw: 29000,
    maxMembers: 10,
    maxProjects: 5,
  },
  PRO: {
    name: "프로",
    priceKrw: 79000,
    maxMembers: 50,
    maxProjects: -1,
  },
  ENTERPRISE: {
    name: "엔터프라이즈",
    priceKrw: 199000,
    maxMembers: -1,
    maxProjects: -1,
  },
};

export function getPlanConfig(plan: Plan): PlanConfig {
  return PLAN_CONFIG[plan];
}
