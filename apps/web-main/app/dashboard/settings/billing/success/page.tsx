import { redirect } from "next/navigation";
import { auth } from "@/auth";
import { prisma } from "@repo/db";
import { issueBillingKey, createSubscription } from "@repo/payments";
import type { Plan } from "@repo/db";

const VALID_PLANS: Plan[] = ["STARTER", "PRO", "ENTERPRISE"];

interface Props {
  searchParams: Promise<{ authKey?: string; customerKey?: string; plan?: string }>;
}

export default async function BillingSuccessPage({ searchParams }: Props) {
  const { authKey, customerKey, plan } = await searchParams;

  if (!authKey || !customerKey || !plan || !VALID_PLANS.includes(plan as Plan)) {
    redirect("/dashboard/settings/billing?error=invalid");
  }

  const session = await auth();
  if (!session?.user?.email) redirect("/login");

  const user = await prisma.user.findUnique({
    where: { email: session.user.email },
    select: { tenantId: true },
  });
  if (!user?.tenantId) redirect("/onboarding");

  try {
    const billing = await issueBillingKey(authKey, customerKey);

    await prisma.tenant.update({
      where: { id: user.tenantId },
      data: {
        tossBillingKey: billing.billingKey,
        tossCustomerKey: customerKey,
      },
    });

    await createSubscription(user.tenantId, plan as Plan);
  } catch {
    redirect("/dashboard/settings/billing?error=payment");
  }

  redirect("/dashboard/settings/billing?success=1");
}
