const TOSS_API_BASE = "https://api.tosspayments.com/v1";

function getAuthHeader(): string {
  const secretKey = process.env.TOSS_SECRET_KEY;
  if (!secretKey) throw new Error("TOSS_SECRET_KEY is not set");
  return `Basic ${Buffer.from(`${secretKey}:`).toString("base64")}`;
}

async function tossRequest<T>(path: string, body?: unknown): Promise<T> {
  const res = await fetch(`${TOSS_API_BASE}${path}`, {
    method: body ? "POST" : "GET",
    headers: {
      Authorization: getAuthHeader(),
      "Content-Type": "application/json",
    },
    ...(body ? { body: JSON.stringify(body) } : {}),
  });

  if (!res.ok) {
    const err = await res.json() as { code: string; message: string };
    throw new Error(`[Toss] ${err.code}: ${err.message}`);
  }

  return res.json() as Promise<T>;
}

export interface TossBillingAuthResponse {
  billingKey: string;
  customerKey: string;
  card: { issuerCode: string; acquirerCode: string; number: string };
}

export interface TossPaymentResponse {
  paymentKey: string;
  orderId: string;
  status: string;
  totalAmount: number;
  approvedAt: string;
}

// 빌링키 발급 (카드 인증 완료 후 authKey로 호출)
export async function issueBillingKey(
  authKey: string,
  customerKey: string
): Promise<TossBillingAuthResponse> {
  return tossRequest<TossBillingAuthResponse>("/billing/authorizations/issue", {
    authKey,
    customerKey,
  });
}

// 빌링키로 정기 결제
export async function chargeBilling(
  billingKey: string,
  {
    customerKey,
    amount,
    orderId,
    orderName,
    customerEmail,
  }: {
    customerKey: string;
    amount: number;
    orderId: string;
    orderName: string;
    customerEmail: string;
  }
): Promise<TossPaymentResponse> {
  return tossRequest<TossPaymentResponse>(`/billing/${billingKey}`, {
    customerKey,
    amount,
    orderId,
    orderName,
    customerEmail,
  });
}

// 결제 취소
export async function cancelPayment(
  paymentKey: string,
  cancelReason: string
): Promise<void> {
  await tossRequest(`/payments/${paymentKey}/cancel`, { cancelReason });
}
