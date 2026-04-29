"use client";

import { useEffect, useRef, useState } from "react";
import type { Plan } from "@repo/db";
import { PLAN_CONFIG } from "@repo/payments";

declare global {
  interface Window {
    TossPayments: (clientKey: string) => {
      requestBillingAuth: (method: string, options: {
        customerKey: string;
        successUrl: string;
        failUrl: string;
      }) => Promise<void>;
    };
  }
}

const UPGRADEABLE_PLANS: Plan[] = ["STARTER", "PRO", "ENTERPRISE"];

interface Props {
  currentPlan: Plan;
  hasCard: boolean;
  clientKey: string;
}

export default function BillingWidget({ currentPlan, hasCard, clientKey }: Props) {
  const [selectedPlan, setSelectedPlan] = useState<Plan | null>(null);
  const [loading, setLoading] = useState(false);
  const [cancelLoading, setCancelLoading] = useState(false);
  const scriptLoaded = useRef(false);

  useEffect(() => {
    if (scriptLoaded.current) return;
    const script = document.createElement("script");
    script.src = "https://js.tosspayments.com/v1/payment";
    script.async = true;
    document.head.appendChild(script);
    scriptLoaded.current = true;
  }, []);

  const handleUpgrade = async () => {
    if (!selectedPlan || !clientKey) return;
    setLoading(true);

    try {
      const customerKey = crypto.randomUUID();
      const origin = window.location.origin;
      const toss = window.TossPayments(clientKey);

      await toss.requestBillingAuth("카드", {
        customerKey,
        successUrl: `${origin}/dashboard/settings/billing/success?plan=${selectedPlan}`,
        failUrl: `${origin}/dashboard/settings/billing?error=card`,
      });
    } catch {
      setLoading(false);
    }
  };

  const handleCancel = async () => {
    if (!confirm("구독을 취소하시겠습니까? 현재 기간 만료 후 FREE 플랜으로 전환됩니다.")) return;
    setCancelLoading(true);
    try {
      await fetch("/api/billing", { method: "DELETE" });
      window.location.reload();
    } finally {
      setCancelLoading(false);
    }
  };

  const upgradeablePlans = UPGRADEABLE_PLANS.filter((p) => p !== currentPlan);

  return (
    <div className="space-y-4">
      {upgradeablePlans.length > 0 && (
        <div className="flex items-center gap-3">
          <select
            value={selectedPlan ?? ""}
            onChange={(e) => setSelectedPlan(e.target.value as Plan)}
            className="rounded-md border border-gray-300 px-3 py-2 text-sm focus:outline-none focus:ring-2 focus:ring-blue-500"
          >
            <option value="" disabled>플랜 선택</option>
            {upgradeablePlans.map((plan) => (
              <option key={plan} value={plan}>
                {PLAN_CONFIG[plan].name} — ₩{PLAN_CONFIG[plan].priceKrw.toLocaleString()}/월
              </option>
            ))}
          </select>
          <button
            onClick={handleUpgrade}
            disabled={!selectedPlan || loading}
            className="rounded-lg bg-blue-600 px-4 py-2 text-sm font-medium text-white hover:bg-blue-700 disabled:opacity-50"
          >
            {loading ? "처리 중..." : hasCard ? "플랜 변경" : "카드 등록 후 구독"}
          </button>
        </div>
      )}

      {currentPlan !== "FREE" && (
        <button
          onClick={handleCancel}
          disabled={cancelLoading}
          className="text-sm text-red-500 hover:text-red-700 underline disabled:opacity-50"
        >
          {cancelLoading ? "처리 중..." : "구독 취소"}
        </button>
      )}
    </div>
  );
}
