FROM node:22-alpine AS base

FROM base AS deps
RUN apk add --no-cache libc6-compat openssl
WORKDIR /app

COPY package.json pnpm-lock.yaml pnpm-workspace.yaml ./
COPY apps/web-partner/package.json ./apps/web-partner/
COPY packages/ui/package.json ./packages/ui/
COPY packages/shared-types/package.json ./packages/shared-types/
COPY packages/shared-config/package.json ./packages/shared-config/
COPY packages/shared-config/typescript-config/package.json ./packages/shared-config/typescript-config/
COPY packages/db/package.json ./packages/db/
COPY packages/auth/package.json ./packages/auth/

RUN npm install -g pnpm@9.0.0
RUN pnpm install --frozen-lockfile

FROM base AS builder
RUN apk add --no-cache openssl
WORKDIR /app
COPY --from=deps /app/node_modules ./node_modules
COPY --from=deps /app/apps/web-partner/node_modules ./apps/web-partner/node_modules
COPY --from=deps /app/packages/db/node_modules ./packages/db/node_modules
COPY --from=deps /app/packages/auth/node_modules ./packages/auth/node_modules
COPY . .

RUN npm install -g pnpm@9.0.0
RUN ./packages/db/node_modules/.bin/prisma generate --schema=packages/db/prisma/schema.prisma
RUN pnpm --filter web-partner build

FROM base AS runner
WORKDIR /app

ENV NODE_ENV=production

RUN addgroup --system --gid 1001 nodejs
RUN adduser --system --uid 1001 nextjs

COPY --from=builder --chown=nextjs:nodejs /app/apps/web-partner/.next/standalone ./
COPY --from=builder --chown=nextjs:nodejs /app/apps/web-partner/public ./apps/web-partner/public
COPY --from=builder --chown=nextjs:nodejs /app/apps/web-partner/.next/static ./apps/web-partner/.next/static

USER nextjs

EXPOSE 3002

ENV PORT=3002
ENV HOSTNAME="0.0.0.0"

CMD ["node", "apps/web-partner/server.js"]
