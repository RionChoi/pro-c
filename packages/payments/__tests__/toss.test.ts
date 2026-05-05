import { describe, it, expect, vi, beforeEach } from "vitest";

// Mock fetch globally
const mockFetch = vi.fn();
vi.stubGlobal("fetch", mockFetch);

// Set env before importing
vi.stubEnv("TOSS_SECRET_KEY", "test_sk_secret123");

describe("toss", () => {
  beforeEach(() => {
    mockFetch.mockReset();
  });

  // Dynamic import to pick up env stub
  async function loadToss() {
    return await import("../src/toss");
  }

  describe("issueBillingKey", () => {
    it("should call Toss API with correct params", async () => {
      const { issueBillingKey } = await loadToss();

      mockFetch.mockResolvedValueOnce({
        ok: true,
        json: async () => ({
          billingKey: "billing_key_123",
          customerKey: "cust_key_456",
          card: { issuerCode: "11", acquirerCode: "11", number: "4242****4242" },
        }),
      });

      const result = await issueBillingKey("auth_key_abc", "cust_key_456");

      expect(mockFetch).toHaveBeenCalledOnce();
      const [url, options] = mockFetch.mock.calls[0] as [string, RequestInit];
      expect(url).toBe("https://api.tosspayments.com/v1/billing/authorizations/issue");
      expect(options.method).toBe("POST");
      expect(options.headers).toHaveProperty("Authorization");

      const body = JSON.parse(options.body as string) as { authKey: string; customerKey: string };
      expect(body.authKey).toBe("auth_key_abc");
      expect(body.customerKey).toBe("cust_key_456");

      expect(result.billingKey).toBe("billing_key_123");
    });

    it("should throw on API error", async () => {
      const { issueBillingKey } = await loadToss();

      mockFetch.mockResolvedValueOnce({
        ok: false,
        json: async () => ({ code: "INVALID_AUTH_KEY", message: "유효하지 않은 인증키" }),
      });

      await expect(issueBillingKey("bad_key", "cust")).rejects.toThrow("INVALID_AUTH_KEY");
    });
  });

  describe("chargeBilling", () => {
    it("should charge with correct billing key and amount", async () => {
      const { chargeBilling } = await loadToss();

      mockFetch.mockResolvedValueOnce({
        ok: true,
        json: async () => ({
          paymentKey: "pay_key_789",
          orderId: "order_123",
          status: "DONE",
          totalAmount: 29000,
          approvedAt: "2026-05-05T10:00:00+09:00",
        }),
      });

      const result = await chargeBilling("billing_key_123", {
        customerKey: "cust_key_456",
        amount: 29000,
        orderId: "order_123",
        orderName: "스타터 플랜 1개월",
        customerEmail: "test@example.com",
      });

      const [url] = mockFetch.mock.calls[0] as [string];
      expect(url).toBe("https://api.tosspayments.com/v1/billing/billing_key_123");
      expect(result.totalAmount).toBe(29000);
      expect(result.paymentKey).toBe("pay_key_789");
    });
  });

  describe("cancelPayment", () => {
    it("should call cancel API with reason", async () => {
      const { cancelPayment } = await loadToss();

      mockFetch.mockResolvedValueOnce({
        ok: true,
        json: async () => ({}),
      });

      await cancelPayment("pay_key_789", "고객 요청");

      const [url, options] = mockFetch.mock.calls[0] as [string, RequestInit];
      expect(url).toBe("https://api.tosspayments.com/v1/payments/pay_key_789/cancel");
      const body = JSON.parse(options.body as string) as { cancelReason: string };
      expect(body.cancelReason).toBe("고객 요청");
    });
  });

  describe("auth header", () => {
    it("should use Base64 encoded secret key", async () => {
      const { issueBillingKey } = await loadToss();

      mockFetch.mockResolvedValueOnce({
        ok: true,
        json: async () => ({
          billingKey: "bk",
          customerKey: "ck",
          card: { issuerCode: "", acquirerCode: "", number: "" },
        }),
      });

      await issueBillingKey("ak", "ck");

      const [, options] = mockFetch.mock.calls[0] as [string, RequestInit];
      const authHeader = (options.headers as Record<string, string>).Authorization;
      expect(authHeader).toMatch(/^Basic /);

      // Decode and verify format: "secretKey:"
      const decoded = Buffer.from(authHeader.replace("Basic ", ""), "base64").toString();
      expect(decoded).toBe("test_sk_secret123:");
    });
  });
});
