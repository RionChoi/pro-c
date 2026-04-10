# Project Specification: Platform

## 🏗 Architecture
The project is structured as a **Monorepo** using **Turborepo** for build orchestration and **pnpm** as the package manager.

- **Workspace Manager:** `pnpm` (v9.0.0)
- **Build System:** `Turborepo` (v2.9.3)
- **CI/CD & Infra:** GitHub Actions, Kubernetes (K8s), Helm

---

## 💻 Tech Stack
| Category | Technology | Version |
| :--- | :--- | :--- |
| **Language** | TypeScript | 5.9.2 |
| **Framework** | Next.js (App Router) | 16.2.0 |
| **Library** | React | 19.2.0 |
| **Auth** | Next-Auth (Auth.js) | v5.0.0-beta.30 |
| **Styling** | CSS Modules / globals.css | - |
| **Linter/Formatter**| ESLint, Prettier | 9.x / 3.x |

---

## 📁 Directory Structure
The repository is divided into `apps`, `packages`, and `services`.

### 📱 Applications (`/apps`)
- **`web-main`**: The primary user-facing application (Next.js).
- **`web-admin`**: Administrative management dashboard (Next.js).
- **`web-partner`**: Partner/B2B portal (Next.js).
- **`docs`**: Project documentation site.

### 📦 Shared Packages (`/packages`)
- **`ui`**: Shared React components (Button, Card, etc.).
- **`shared-types`**: Common TypeScript interfaces and types.
- **`shared-config`**: Unified configurations for ESLint, TypeScript, and Next.js.

### ⚙️ Services (`/services`)
*Planned microservices architecture:*
- **`api-gateway`**: Centralized entry point.
- **`auth-service`**: Authentication and authorization logic.
- **`user-service`**: User data management.

---

## 🛠 Key Scripts
Run these from the root directory using `pnpm`:
- `pnpm dev`: Runs all applications in development mode.
- `pnpm build`: Builds all projects using Turbo cache.
- `pnpm lint`: Runs linting across the workspace.
- `pnpm check-types`: Static type checking for all packages.

---

> [!IMPORTANT]
> **Branch Insight:**
> The workspace is currently on the `master` branch, which contains the directory structure but lacks several core files. The complete source code and configurations (including `package.json`, `pnpm-workspace.yaml`, and `turbo.json`) are maintained in the **`main`** branch.
