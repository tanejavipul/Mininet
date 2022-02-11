public class Complexity {
	public static void main(String [] args){
		String s="this is a test";
		String t="that is a test";

		System.out.println("Begin O(n log(n)) loop");
		for(int i=0;i<100000;i++){
			if(i%1000==0)System.out.println(i);
			for(int j=0;j<=1+Math.log(i)/Math.log(2);j++){
				if(s.equals(t)){
					int k=3;
				}
			}
		}
		System.out.println("End O(n log(n)) loop");
		System.out.println("Begin O(n^2) loop");
		for(int i=0;i<100000;i++){
			if(i%1000==0)System.out.println(i);
			for(int j=0;j<i;j++){
				if(s.equals(t)){
					int k=3;
				}
			}
		}
		System.out.println("End O(n^2) loop");
	}
}
