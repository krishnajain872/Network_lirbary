# examples/oms/frontend/Dockerfile
FROM node:18-alpine AS deps
WORKDIR /app
COPY package.json ./
# Use install instead of ci because package-lock.json might not exist yet
RUN --mount=type=cache,target=/root/.npm npm install

FROM node:18-alpine AS builder
WORKDIR /app
COPY --from=deps /app/node_modules ./node_modules
COPY . .
RUN npm run build

FROM node:18-alpine AS runner
WORKDIR /app
ENV NODE_ENV production
COPY --from=builder /app/public ./public
COPY --from=builder /app/.next/standalone ./
COPY --from=builder /app/.next/static ./.next/static

EXPOSE 3000
CMD ["node", "server.js"]
