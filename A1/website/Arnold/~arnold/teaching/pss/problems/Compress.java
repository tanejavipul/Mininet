class Compress {
	public static void main(String [] args){
		testCompressors(""); //
		testCompressors("a"); //1a
		testCompressors("aa"); //1a
		testCompressors("aaa"); //1a
		testCompressors("aaab"); //1a
		testCompressors("abaa"); //1a
		testCompressors("aaaaabbccccdeeeeeeef"); //5a2b4c1d7e1f
	}
	public static void testCompressors(String s){
		System.out.println("Compressing "+s);
		System.out.println(compress1(s));
		System.out.println(compress2(s));
		System.out.println(compress3(s));
		System.out.println(compress4(s));
		System.out.println(compress5(s));
		System.out.println(compress6(s));
	}

	public static String compress1(String s){
		String t="";
		while(s.length()>0){
			int rl=runLength(s);
			t=t+rl+s.charAt(0);
			s=s.substring(rl);
		}
		return(t);
	}
	public static int runLength(String s){
		int i=0;
		for(i=0; i<s.length() && s.charAt(i)==s.charAt(0);i++);
		return i;
	}
	public static String compress2(String s){
		String t="";

		int count=1, i=1; 
		while(i<=s.length()){
			if(i==s.length() || s.charAt(i)!=s.charAt(i-1)){
				t=t+count+s.charAt(i-1);
				count=1;
			} else count++;
			i++;
		}
		return(t);
	}

	public static String compress3(String s){
		String t="";
		for(int i=1, count=1;i<=s.length();i++){
			if(i==s.length() || s.charAt(i)!=s.charAt(i-1)){
				t=t+count+s.charAt(i-1);
				count=1;
			} else count++;
		}
		return(t);
	}

	public static String compress4(String s){
		String t="";
		int i=0; 
		while(i<s.length()){
			int j=i;
			while(i<s.length() && s.charAt(j)==s.charAt(i))i=i+1;
			t=t+((i-j))+s.charAt(j);
		}
		return(t);
	}

	public static String compress5(String s){
		String t="";
		int i,j;
		for(j=0;j<s.length();j=i){
			for(i=j;i<s.length() && s.charAt(j)==s.charAt(i) ;i++);
			t=t+((i-j))+s.charAt(j);
		}
		return(t);
	}

	public static int runLength2(String s){
		if(s.length()>=2 && s.charAt(0)==s.charAt(1))return(1+runLength2(s.substring(1)));
		else return((s.length()==0)?0:1);
	}

	public static String compress6(String s){
		if(s.length()==0)return "";
		return(""+runLength2(s)+s.charAt(0)+compress6(s.substring(runLength2(s))));
	}
}
