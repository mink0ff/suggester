package main

import (
    "fmt"
    "net/http"
    "sync"
    "time"
)

func main() {
    var (
        totalRequests   int64
        successRequests int64
        failedRequests  int64
        totalDuration   time.Duration
        mu              sync.Mutex
    )

    url := "http://localhost:39757/suggest?word=a"
    concurrency := 50       // количество параллельных горутин
    totalRuns := 100       // общее количество запросов

    var wg sync.WaitGroup
    jobs := make(chan struct{}, concurrency)

    start := time.Now()

    for i := 0; i < totalRuns; i++ {
        wg.Add(1)
        jobs <- struct{}{}

        go func() {
            defer wg.Done()
            reqStart := time.Now()
            resp, err := http.Get(url)
            duration := time.Since(reqStart)

            mu.Lock()
            totalRequests++
            totalDuration += duration
            if err != nil || resp.StatusCode != http.StatusOK {
                failedRequests++
            } else {
                successRequests++
            }
            mu.Unlock()

            if resp != nil {
                resp.Body.Close()
            }
            <-jobs
        }()
    }

    wg.Wait()
    elapsed := time.Since(start)

    fmt.Println("=== Load test finished ===")
    fmt.Printf("Total requests: %d\n", totalRequests)
    fmt.Printf("Successful requests: %d\n", successRequests)
    fmt.Printf("Failed requests: %d\n", failedRequests)
    fmt.Printf("Total time: %s\n", elapsed)
    avgDuration := time.Duration(0)
    if totalRequests > 0 {
        avgDuration = totalDuration / time.Duration(totalRequests)
    }
    fmt.Printf("Average request duration: %s\n", avgDuration)
}
