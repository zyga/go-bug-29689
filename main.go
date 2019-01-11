// -*- Mode: Go; indent-tabs-mode: t -*-

package main

import (
	"fmt"
	"os"
)

func main() {
	if err := BootstrapError(); err != nil {
		fmt.Printf("error: %s\n", err)
		os.Exit(1)
	}
}
