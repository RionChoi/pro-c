export type Role = "ADMIN" | "MEMBER" | "PARTNER" | "VIEWER";

// ADMIN은 항상 통과 (super-user). roles를 빈 배열로 넘기면 "로그인만 확인".
export function hasRole(userRole: string | null | undefined, ...required: Role[]): boolean {
  if (!userRole) return false;
  if (userRole === "ADMIN") return true;
  return required.length === 0 || required.includes(userRole as Role);
}
