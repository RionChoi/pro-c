FROM node:22-alpine AS base

FROM base AS deps
RUN apk add --no-cache libc6-compat
WORKDIR /app

COPY package.json pnpm-lock.yaml pnpm-workspace.yaml ./
COPY apps/web-main/package.json ./apps/web-main/
COPY packages/ui/package.json ./packages/ui/
COPY packages/shared-types/package.json ./packages/shared-types/
COPY packages/shared-config/package.json ./packages/shared-config/
COPY packages/shared-config/typescript-config/package.json ./packages/shared-config/typescript-config/

RUN npm install -g pnpm@9.0.0
RUN pnpm install --frozen-lockfile

FROM base AS builder
WORKDIR /app
COPY --from=deps /app/node_modules ./node_modules
COPY --from=deps /app/apps/web-main/node_modules ./apps/web-main/node_modules
COPY . .

RUN npm install -g pnpm@9.0.0
RUN pnpm --filter web-main exec prisma generate
RUN pnpm --filter web-main build

FROM base AS runner
WORKDIR /app

ENV NODE_ENV=production

RUN addgroup --system --gid 1001 nodejs
RUN adduser --system --uid 1001 nextjs

COPY --from=builder --chown=nextjs:nodejs /app/apps/web-main/.next/standalone ./
COPY --from=builder --chown=nextjs:nodejs /app/apps/web-main/public ./apps/web-main/public
COPY --from=builder --chown=nextjs:nodejs /app/apps/web-main/.next/static ./apps/web-main/.next/static

USER nextjs

EXPOSE 3000

ENV PORT=3000
ENV HOSTNAME="0.0.0.0"

CMD ["node", "apps/web-main/server.js"]
