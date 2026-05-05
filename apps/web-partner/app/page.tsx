import { auth } from "@/auth";
import { redirect } from "next/navigation";

export default async function PartnerHomePage() {
  const session = await auth();
  if (!session?.user?.email) redirect("/login");

  // 인증된 파트너는 대시보드로 이동
  redirect("/dashboard");
}
