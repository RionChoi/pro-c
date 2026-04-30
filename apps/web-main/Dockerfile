FROM node:22-alpine AS base

FROM base AS deps
RUN apk add --no-cache libc6-compat openssl
WORKDIR /app

COPY package.json pnpm-lock.yaml pnpm-workspace.yaml .npmrc ./
COPY apps/web-main/package.json ./apps/web-main/
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
COPY . .

RUN npm install -g pnpm@9.0.0
RUN node_modules/.bin/prisma generate --schema=packages/db/prisma/schema.prisma
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

EXPOSE 3003

ENV PORT=3003
ENV HOSTNAME="0.0.0.0"

CMD ["node", "apps/web-main/server.js"]
