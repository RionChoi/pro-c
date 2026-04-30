// Query Contract — 읽기 전용
export interface QueryResult<T> {
  data: T;
  pagination?: {
    total: number;
    page: number;
    limit: number;
    hasNext: boolean;
  };
  timestamp: string;
}

// Command Contract — 쓰기 전용
export interface CommandResult<T = void> {
  success: boolean;
  data?: T;
  error?: import("./api").ApiError;
  affectedCount?: number;
}

// Eventual Consistency Marker
export interface AsyncCommandResult {
  jobId: string;
  status: "pending" | "processing" | "completed" | "failed";
  estimatedCompletion?: string;
}
