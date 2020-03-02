void setBuildStatus(String message, String state) {
	step([
		$class: "GitHubCommitStatusSetter",
		reposSource: [$class: "ManuallyEnteredRepositorySource", url: env.GIT_URL],
		commitShaSource: [$class: "ManuallyEnteredShaSource", sha: env.GIT_COMMIT],
		contextSource: [$class: "ManuallyEnteredCommitContextSource", context: "ci/jenkins/build-status"],
		errorHandlers: [[$class: "ChangingBuildStatusErrorHandler", result: "UNSTABLE"]],
		statusResultSource: [ $class: "ConditionalStatusResultSource", results: [[$class: "AnyBuildResult", message: message, state: state]] ]
	]);
}

pipeline {
	agent any
	stages {
		stage('Notify GitHub') {
			steps {
				setBuildStatus('Build is pending', 'PENDING')
			}
		}
		stage('Update Git submodules') {
			steps {
				sh 'git submodule update --init --recursive'
			}
		}
		stage('Build') {
			steps {
				sh 'mkdir -p cmake-build-release'
				sh 'cd cmake-build-release && cmake -DCMAKE_BUILD_TYPE=Release .. && make nba_libretro'
			}
		}
	}
	post {
		always {
			archiveArtifacts artifacts: 'cmake-build-release/*.dll,cmake-build-release/*.so', onlyIfSuccessful: true
		}
		success {
			setBuildStatus('Build succeeded', 'SUCCESS')
		}
		failure {
			setBuildStatus('Build failed', 'FAILURE')
		}
		unstable {
			setBuildStatus('Build is unstable', 'UNSTABLE')
		}
	}
}